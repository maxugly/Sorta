
/**
 * @file ControlPanel.cpp
 */
#include "UI/ControlPanel.h"
#include "Core/AudioPlayer.h"
#include "MainComponent.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/ControlButtonsPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "Presenters/CutPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/PlaybackRepeatController.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Presenters/MatrixPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/ZoomPresenter.h"
#include "UI/FocusManager.h"
#include "UI/LayoutManager.h"
#include "UI/Views/CutLayerView.h"
#include "UI/Views/OverlayView.h"
#include "UI/Views/PlaybackCursorGlow.h"
#include "UI/Views/PlaybackCursorView.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/ZoomView.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"
#include "Utils/TimeUtils.h"
#include <cmath>

ControlPanel::ControlPanel(MainComponent &ownerComponent, SessionState &sessionStateIn)
    : owner(ownerComponent), sessionState(sessionStateIn), modernLF(),
      layoutManager(std::make_unique<LayoutManager>(*this)) {
    initialiseLookAndFeel();
    setupViews();
    setupStrips();

    setMouseCursor(juce::MouseCursor::CrosshairCursor);
}

void ControlPanel::injectLogic(InteractionCoordinator& ic, PlaybackTimerManager& ptm, PresenterCore& pc, FocusManager& fm) {
    interactionCoordinator = &ic;
    playbackTimerManager = &ptm;
    presenterCore = &pc;
    focusManager = &fm;

    setupListeners();
    getPresenterCore().getControlStatePresenter().updateUIFromState();
    finaliseSetup();
#if !defined(JUCE_HEADLESS)
    playbackTimerManager->attachToVBlank(this);
#endif
}

void ControlPanel::setupViews() {
    waveformCanvasView = std::make_unique<WaveformCanvasView>(*this);
    addAndMakeVisible(waveformCanvasView.get());

    auto& cutLayer = waveformCanvasView->getCutLayerView();
    cutLayer.updateState({}); // Initial empty state

    overlayView = std::make_unique<OverlayView>(*this);
    addAndMakeVisible(overlayView.get());

    topBarView = std::make_unique<TopBarView>(*this);
    addAndMakeVisible(topBarView.get());

    playbackTimeView = std::make_unique<PlaybackTimeView>();
    addAndMakeVisible(playbackTimeView.get());

    fpsView = std::make_unique<FpsView>();
    addAndMakeVisible(fpsView.get());
    fpsView->toFront(false);
}

void ControlPanel::setupStrips() {
    inStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::In);
    addAndMakeVisible(inStrip.get());
    
    outStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::Out);
    addAndMakeVisible(outStrip.get());

    // --- NEW: Instantiate the CutLengthStrip ---
    cutLengthStrip = std::make_unique<CutLengthStrip>();
    addAndMakeVisible(cutLengthStrip.get());
}

juce::MouseCursor ControlPanel::getMouseCursor() {
    if (getInteractionCoordinator().getPlacementMode() != AppEnums::PlacementMode::None)
        return juce::MouseCursor::PointingHandCursor;

    return juce::MouseCursor::CrosshairCursor;
}

void ControlPanel::setupListeners() {
    playbackTimerManager->addListener(&getPresenterCore().getZoomPresenter());
    playbackTimerManager->addListener(&getPlaybackTextPresenter());

    playbackTimerManager->setZoomPointProvider([this]() {
        auto dragged = getMarkerMouseHandler().getDraggedHandle();
        if (dragged == MarkerMouseHandler::CutMarkerHandle::In)
            return AppEnums::ActiveZoomPoint::In;
        if (dragged == MarkerMouseHandler::CutMarkerHandle::Out)
            return AppEnums::ActiveZoomPoint::Out;
        return AppEnums::ActiveZoomPoint::None;
    });

    inStrip->setPresenter(&getBoundaryLogicPresenter());
    outStrip->setPresenter(&getBoundaryLogicPresenter());

    addMouseListener(&getMarkerMouseHandler(), true);
    addMouseListener(&getWaveformMouseHandler(), true);
}

ControlPanel::~ControlPanel() {
    setLookAndFeel(nullptr);
}

void ControlPanel::initialiseLookAndFeel() {
    setLookAndFeel(&modernLF);
    modernLF.setBaseOffColor(Config::Colors::Button::base);
    modernLF.setBaseOnColor(Config::Colors::Button::on);
    modernLF.setTextColor(Config::Colors::Button::text);
    modernLF.setTextActiveColor(Config::Colors::Button::textActive);

    modernLF.setColour(juce::ComboBox::backgroundColourId, Config::Colors::Button::base);
    modernLF.setColour(juce::ComboBox::outlineColourId, Config::Colors::Button::outline);
    modernLF.setColour(juce::ComboBox::textColourId, Config::Colors::Button::text);
    modernLF.setColour(juce::ComboBox::arrowColourId, Config::Colors::Button::text);

    modernLF.setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    modernLF.setColour(juce::TextEditor::outlineColourId, Config::Colors::Button::outline);
}

void ControlPanel::refreshThemeLive() {
    initialiseLookAndFeel();
    if (waveformCanvasView) waveformCanvasView->getWaveformView().clearCaches();
    
    auto& stats = getPresenterCore().getStatsPresenter();
    stats.getDisplay().setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    stats.getDisplay().setColour(juce::TextEditor::textColourId, Config::Colors::statsText);
    stats.updateStats();

    if (playbackTimeView) {
        playbackTimeView->getElapsedEditor().setCustomTextColor(Config::Colors::playbackText);
        playbackTimeView->getRemainingEditor().setCustomTextColor(Config::Colors::playbackText);
        playbackTimeView->getTotalTimeEditor().setCustomTextColor(Config::Colors::totalTimeText);
    }
    if (inStrip) {
        inStrip->getTimerEditor().setCustomTextColor(Config::Colors::cutText);
        inStrip->getThresholdEditor().setCustomTextColor(Config::Colors::thresholdText);
    }
    if (outStrip) {
        outStrip->getTimerEditor().setCustomTextColor(Config::Colors::cutText);
        outStrip->getThresholdEditor().setCustomTextColor(Config::Colors::thresholdText);
    }
    if (cutLengthStrip) {
        cutLengthStrip->getLengthEditor().setCustomTextColor(Config::Colors::cutText);
    }

    if (topBarView) {
        topBarView->openButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->openButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        topBarView->modeButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->modeButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        topBarView->statsButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->statsButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        topBarView->channelViewButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->channelViewButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        topBarView->themeUpButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->themeUpButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        topBarView->themeDownButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        topBarView->themeDownButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);

        topBarView->exitButton.setColour(juce::TextButton::textColourOffId, Config::Colors::Button::exitText);
        topBarView->exitButton.setColour(juce::TextButton::textColourOnId, Config::Colors::Button::exitText);

        if (auto* ts = topBarView->transportStrip.get()) {
            ts->getPlayStopButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
            ts->getPlayStopButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
            ts->getStopButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
            ts->getStopButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
            ts->getAutoplayButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
            ts->getAutoplayButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
            ts->getRepeatButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
            ts->getRepeatButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
            ts->getCutButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
            ts->getCutButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        }
    }

    if (inStrip) {
        inStrip->getMarkerButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        inStrip->getMarkerButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        inStrip->getResetButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        inStrip->getResetButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        inStrip->getAutoCutButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        inStrip->getAutoCutButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        inStrip->getLockButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        inStrip->getLockButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
    }
    if (outStrip) {
        outStrip->getMarkerButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        outStrip->getMarkerButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        outStrip->getResetButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        outStrip->getResetButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        outStrip->getAutoCutButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        outStrip->getAutoCutButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        outStrip->getLockButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        outStrip->getLockButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
    }
    if (cutLengthStrip) {
        cutLengthStrip->getLockButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        cutLengthStrip->getLockButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
    }

    sendLookAndFeelChange();
    repaint();
}

void ControlPanel::invokeOwnerOpenDialog() {
    owner.openButtonClicked();
}

void ControlPanel::finaliseSetup() {
    getPlaybackTextPresenter().initialiseEditors();

    getPresenterCore().getControlStatePresenter().refreshStates();
    getPresenterCore().getMatrixPresenter().fullMatrixUpdate();
    refreshThemeLive();
}

void ControlPanel::resized() {
    if (layoutManager != nullptr)
        layoutManager->performLayout();

    if (waveformCanvasView != nullptr)
        waveformCanvasView->setBounds(layoutCache.waveformBounds);

    if (overlayView != nullptr)
        overlayView->setBounds(getLocalBounds());

    if (fpsView != nullptr) {
        fpsView->setVisible(Config::Advanced::showFpsOverlay);
        if (Config::Advanced::showFpsOverlay) {
            int x = Config::Advanced::fpsOverlayX;
            int y = Config::Advanced::fpsOverlayY;
            
            if (Config::Advanced::fpsOverlayPosition == Config::Advanced::posTopCenter) {
                x = (getWidth() - Config::Layout::Fps::width) / 2;
                y = Config::Layout::Fps::margin;
            } else if (Config::Advanced::fpsOverlayPosition == Config::Advanced::posBottomCenter) {
                x = (getWidth() - Config::Layout::Fps::width) / 2;
                y = getHeight() - Config::Layout::Fps::height - Config::Layout::Fps::margin;
            }
            
            fpsView->setBounds(x, y, Config::Layout::Fps::width, Config::Layout::Fps::height);
        }
    }
}

void ControlPanel::paint(juce::Graphics &g) {
    g.fillAll(Config::Colors::Window::background);
}



AudioPlayer &ControlPanel::getAudioPlayer() {
    return *owner.getAudioPlayer();
}

AudioPlayer &ControlPanel::getAudioPlayer() const {
    return *owner.getAudioPlayer();
}

const MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() const {
    return getPresenterCore().getCutPresenter().getMarkerMouseHandler();
}

MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() {
    return getPresenterCore().getCutPresenter().getMarkerMouseHandler();
}

const WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() const {
    return getPresenterCore().getCutPresenter().getWaveformMouseHandler();
}

WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() {
    return getPresenterCore().getCutPresenter().getWaveformMouseHandler();
}

juce::TextEditor &ControlPanel::getStatsDisplay() {
    return getPresenterCore().getStatsPresenter().getDisplay();
}

const juce::LookAndFeel &ControlPanel::getLookAndFeel() const {
    return modernLF;
}
