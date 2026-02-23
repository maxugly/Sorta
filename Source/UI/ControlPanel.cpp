
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

    matrixView = std::make_unique<MatrixView>();
    addAndMakeVisible(matrixView.get());
}

void ControlPanel::setupStrips() {
    inStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::In);
    addAndMakeVisible(inStrip.get());

    outStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::Out);
    addAndMakeVisible(outStrip.get());
}

juce::MouseCursor ControlPanel::getMouseCursor() {
    if (getInteractionCoordinator().getPlacementMode() != AppEnums::PlacementMode::None)
        return juce::MouseCursor::PointingHandCursor;

    return juce::MouseCursor::CrosshairCursor;
}

void ControlPanel::setupListeners() {
    playbackTimerManager->addListener(&waveformCanvasView->getPlaybackCursorView());
    playbackTimerManager->addListener(&getPresenterCore().getZoomPresenter());
    playbackTimerManager->addListener(overlayView.get());
    playbackTimerManager->addListener(&getBoundaryLogicPresenter());
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
    if (playbackTimerManager != nullptr) {
        playbackTimerManager->stopTimer();
    }

    setLookAndFeel(nullptr);
}

void ControlPanel::initialiseLookAndFeel() {
    setLookAndFeel(&modernLF);
    modernLF.setBaseOffColor(Config::Colors::Button::base);
    modernLF.setBaseOnColor(Config::Colors::Button::on);
    modernLF.setTextColor(Config::Colors::Button::text);
}

void ControlPanel::invokeOwnerOpenDialog() {
    owner.openButtonClicked();
}

void ControlPanel::finaliseSetup() {
    getPlaybackTextPresenter().initialiseEditors();

    getPresenterCore().getControlStatePresenter().refreshStates();
}

void ControlPanel::resized() {
    if (layoutManager != nullptr)
        layoutManager->performLayout();

    if (waveformCanvasView != nullptr)
        waveformCanvasView->setBounds(layoutCache.waveformBounds);

    if (overlayView != nullptr)
        overlayView->setBounds(getLocalBounds());
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
