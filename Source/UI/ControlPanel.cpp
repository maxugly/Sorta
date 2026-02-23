

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
      silenceDetector(std::make_unique<SilenceDetector>(*this)),
      layoutManager(std::make_unique<LayoutManager>(*this)),
      focusManager(std::make_unique<FocusManager>(*this)) {
    initialiseLookAndFeel();
    setupCoreComponents();
    setupViews();
    setupStrips();
    setupPresenters();
    setupListeners();

    sessionState.addListener(this);
    updateUIFromState();
    finaliseSetup();

    setMouseCursor(juce::MouseCursor::CrosshairCursor);
}

void ControlPanel::setupCoreComponents() {
    interactionCoordinator = std::make_unique<InteractionCoordinator>();
    playbackTimerManager = std::make_unique<PlaybackTimerManager>(sessionState, getAudioPlayer(),
                                                                  *interactionCoordinator);
}

void ControlPanel::setupViews() {
    waveformCanvasView = std::make_unique<WaveformCanvasView>(*this);
    addAndMakeVisible(waveformCanvasView.get());

    auto& cutLayer = waveformCanvasView->getCutLayerView();
    cutPresenter = std::make_unique<CutPresenter>(*this, sessionState, cutLayer);
    cutLayer.setMarkerMouseHandler(getMarkerMouseHandler());

    overlayView = std::make_unique<OverlayView>(*this);
    addAndMakeVisible(overlayView.get());

    topBarView = std::make_unique<TopBarView>(*this);
    addAndMakeVisible(topBarView.get());

    playbackTimeView = std::make_unique<PlaybackTimeView>();
    addAndMakeVisible(playbackTimeView.get());
}

void ControlPanel::setupStrips() {
    inStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::In, getAudioPlayer(),
                                            sessionState, *silenceDetector);
    inStrip->onMarkerRightClick = [this] {
        setPlacementMode(AppEnums::PlacementMode::CutIn);
        updateCutButtonColors();
        repaint();
    };
    addAndMakeVisible(inStrip.get());

    outStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::Out, getAudioPlayer(),
                                             sessionState, *silenceDetector);
    outStrip->onMarkerRightClick = [this] {
        setPlacementMode(AppEnums::PlacementMode::CutOut);
        updateCutButtonColors();
        repaint();
    };
    addAndMakeVisible(outStrip.get());
}

void ControlPanel::setupPresenters() {
    presenterCore = std::make_unique<PresenterCore>(*this);
}

void ControlPanel::setupListeners() {
    playbackTimerManager->addListener(&waveformCanvasView->getPlaybackCursorView());
    playbackTimerManager->addListener(&waveformCanvasView->getZoomView());
    playbackTimerManager->addListener(&waveformCanvasView->getCutLayerView());
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
}

ControlPanel::~ControlPanel() {
    if (playbackTimerManager != nullptr) {
        playbackTimerManager->stopTimer();
    }

    sessionState.removeListener(this);

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

    updateComponentStates();
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
    getPlaybackTextPresenter().render(g);
}

void ControlPanel::updatePlayButtonText(bool isPlaying) {
    if (auto* ts = getTransportStrip())
        ts->updatePlayButtonText(isPlaying);
}

void ControlPanel::refreshLabels() {
    getBoundaryLogicPresenter().refreshLabels();
    getPlaybackTextPresenter().updateEditors();
}

void ControlPanel::cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
    m_isCutModeActive = prefs.active;

    if (auto* ts = getTransportStrip()) {
        ts->updateAutoplayState(prefs.autoplay);
        ts->updateCutModeState(prefs.active);
    }

    if (inStrip != nullptr)
        inStrip->updateAutoCutState(prefs.autoCut.inActive);
    if (outStrip != nullptr)
        outStrip->updateAutoCutState(prefs.autoCut.outActive);

    if (silenceDetector != nullptr) {
        silenceDetector->setIsAutoCutInActive(prefs.autoCut.inActive);
        silenceDetector->setIsAutoCutOutActive(prefs.autoCut.outActive);
    }

    updateComponentStates();
    repaint();
}

void ControlPanel::cutInChanged(double value) {
    juce::ignoreUnused(value);
    repaint();
}

void ControlPanel::cutOutChanged(double value) {
    juce::ignoreUnused(value);
    repaint();
}

void ControlPanel::jumpToCutIn() {
    getAudioPlayer().setPlayheadPosition(getCutInPosition());
    interactionCoordinator->setNeedsJumpToCutIn(false);
}

double ControlPanel::getCutInPosition() const {
    return sessionState.getCutIn();
}

double ControlPanel::getCutOutPosition() const {
    return sessionState.getCutOut();
}

void ControlPanel::setCutInPosition(double pos) {
    sessionState.setCutIn(pos);
}

void ControlPanel::setCutOutPosition(double pos) {
    sessionState.setCutOut(pos);
}

void ControlPanel::updateComponentStates() {
    getPresenterCore().getControlStatePresenter().refreshStates();
}

void ControlPanel::updateUIFromState() {
    const auto &prefs = sessionState.getCutPrefs();
    const auto &autoCut = prefs.autoCut;

    m_isCutModeActive = prefs.active;

    if (auto* ts = getTransportStrip()) {
        ts->updateAutoplayState(prefs.autoplay);
        ts->updateCutModeState(prefs.active);
    }

    if (inStrip != nullptr)
        inStrip->updateAutoCutState(autoCut.inActive);
    if (outStrip != nullptr)
        outStrip->updateAutoCutState(autoCut.outActive);

    silenceDetector->setIsAutoCutInActive(autoCut.inActive);
    silenceDetector->setIsAutoCutOutActive(autoCut.outActive);

    const int inPercent = static_cast<int>(autoCut.thresholdIn * 100.0f);
    const int outPercent = static_cast<int>(autoCut.thresholdOut * 100.0f);
    silenceDetector->getInSilenceThresholdEditor().setText(juce::String(inPercent),
                                                           juce::dontSendNotification);
    silenceDetector->getOutSilenceThresholdEditor().setText(juce::String(outPercent),
                                                            juce::dontSendNotification);

    updateComponentStates();

    getBoundaryLogicPresenter().refreshLabels();
    getPlaybackTextPresenter().updateEditors();

    if (waveformCanvasView != nullptr)
        waveformCanvasView->getZoomView().repaint();

    repaint();
}

void ControlPanel::setAutoCutInActive(bool isActive) {
    sessionState.setAutoCutInActive(isActive);
}

void ControlPanel::setAutoCutOutActive(bool isActive) {
    sessionState.setAutoCutOutActive(isActive);
}

void ControlPanel::toggleViewMode() {
    currentMode = (currentMode == AppEnums::ViewMode::Classic) ? AppEnums::ViewMode::Overlay
                                                              : AppEnums::ViewMode::Classic;

    if (topBarView != nullptr) {
        topBarView->modeButton.setToggleState(currentMode == AppEnums::ViewMode::Overlay,
                                              juce::dontSendNotification);
        topBarView->modeButton.setButtonText(currentMode == AppEnums::ViewMode::Classic
                                                 ? Config::Labels::viewModeClassic
                                                 : Config::Labels::viewModeOverlay);
    }

    resized();
    repaint();
}

void ControlPanel::toggleChannelViewMode() {
    currentChannelViewMode = (currentChannelViewMode == AppEnums::ChannelViewMode::Mono)
                                 ? AppEnums::ChannelViewMode::Stereo
                                 : AppEnums::ChannelViewMode::Mono;

    if (topBarView != nullptr) {
        topBarView->channelViewButton.setToggleState(
            currentChannelViewMode == AppEnums::ChannelViewMode::Stereo, juce::dontSendNotification);
        topBarView->channelViewButton.setButtonText(
            currentChannelViewMode == AppEnums::ChannelViewMode::Mono
                ? Config::Labels::channelViewMono
                : Config::Labels::channelViewStereo);
    }

    if (waveformCanvasView != nullptr)
        waveformCanvasView->getWaveformView().setChannelMode(currentChannelViewMode);

    repaint();
}

void ControlPanel::setStatsDisplayText(const juce::String &text, juce::Colour color) {
    getPresenterCore().getStatsPresenter().setDisplayText(text, color);
}

void ControlPanel::logStatusMessage(const juce::String &message, bool isError) {
    const auto color = isError ? Config::Colors::statsErrorText : Config::Colors::statsText;

    setStatsDisplayText(message, color);
}

void ControlPanel::updateStatsFromAudio() {
    getPresenterCore().getStatsPresenter().updateStats();
}

void ControlPanel::ensureCutOrder() {
    getBoundaryLogicPresenter().ensureCutOrder();
}

void ControlPanel::setShouldShowStats(bool shouldShowStatsParam) {
    getPresenterCore().getStatsPresenter().setShouldShowStats(shouldShowStatsParam);
}

void ControlPanel::setTotalTimeStaticString(const juce::String &timeString) {
    getPlaybackTextPresenter().setTotalTimeStaticString(timeString);
}

void ControlPanel::updateCutButtonColors() {
    getPresenterCore().getCutButtonPresenter().updateColours();
}

AudioPlayer &ControlPanel::getAudioPlayer() {
    return *owner.getAudioPlayer();
}

AudioPlayer &ControlPanel::getAudioPlayer() const {
    return *owner.getAudioPlayer();
}

const MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() const {
    return cutPresenter->getMarkerMouseHandler();
}

MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() {
    return cutPresenter->getMarkerMouseHandler();
}

const WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() const {
    return cutPresenter->getWaveformMouseHandler();
}

WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() {
    return cutPresenter->getWaveformMouseHandler();
}

juce::TextEditor &ControlPanel::getStatsDisplay() {
    return getPresenterCore().getStatsPresenter().getDisplay();
}

juce::String ControlPanel::formatTime(double seconds) const {
    return TimeUtils::formatTime(seconds);
}

const juce::LookAndFeel &ControlPanel::getLookAndFeel() const {
    return modernLF;
}

void ControlPanel::mouseMove(const juce::MouseEvent &event) {
    getMarkerMouseHandler().mouseMove(event);
    getWaveformMouseHandler().mouseMove(event);
}

void ControlPanel::mouseDown(const juce::MouseEvent &event) {
    getMarkerMouseHandler().mouseDown(event);
    getWaveformMouseHandler().mouseDown(event);
}

void ControlPanel::mouseDrag(const juce::MouseEvent &event) {
    getMarkerMouseHandler().mouseDrag(event);
    getWaveformMouseHandler().mouseDrag(event);
}

void ControlPanel::mouseUp(const juce::MouseEvent &event) {
    getMarkerMouseHandler().mouseUp(event);
    getWaveformMouseHandler().mouseUp(event);
}

void ControlPanel::mouseExit(const juce::MouseEvent &event) {
    getMarkerMouseHandler().mouseExit(event);
    getWaveformMouseHandler().mouseExit(event);
}

void ControlPanel::mouseWheelMove(const juce::MouseEvent &event,
                                  const juce::MouseWheelDetails &wheel) {
    if (wheel.deltaY == 0.0f)
        return;
    getWaveformMouseHandler().mouseWheelMove(event, wheel);
}
