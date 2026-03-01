#include "UI/ControlPanel.h"
#include "Core/AudioPlayer.h"
#include "MainComponent.h"
#include "UI/LayoutManager.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/OverlayView.h"
#include "UI/Views/PlaybackTimeView.h"
#include "UI/Views/FpsView.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/MatrixPresenter.h"
#include "Presenters/CutPresenter.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/ZoomPresenter.h"
#include "Presenters/ThemePresenter.h"
#include "Utils/Config.h"

ControlPanel::ControlPanel(MainComponent &o, SessionState &s) : owner(o), sessionState(s), modernLF(), layoutManager(std::make_unique<LayoutManager>(*this)) {
    addAndMakeVisible(fileQueuePlaceholder); addAndMakeVisible(directoryRoutingView); addAndMakeVisible(exitButton);
    verticalResizer = std::make_unique<XPResizerBar>(&verticalLayoutManager, 1, false); addAndMakeVisible(verticalResizer.get());
    horizontalResizer = std::make_unique<XPResizerBar>(&horizontalLayoutManager, 1, true); addAndMakeVisible(horizontalResizer.get());
    verticalLayoutManager.setItemLayout(0, -0.3, -1.0, -0.7); verticalLayoutManager.setItemLayout(1, 8, 8, 8); verticalLayoutManager.setItemLayout(2, -0.1, -0.5, -0.3);
    horizontalLayoutManager.setItemLayout(0, -0.2, -1.0, -0.85); horizontalLayoutManager.setItemLayout(1, 8, 8, 8); horizontalLayoutManager.setItemLayout(2, -0.05, -0.4, -0.15);
    initialiseLookAndFeel(); setupViews(); setupStrips(); setMouseCursor(juce::MouseCursor::CrosshairCursor);
}

void ControlPanel::injectLogic(InteractionCoordinator& ic, PlaybackTimerManager& ptm, PresenterCore& pc, FocusManager& fm) {
    interactionCoordinator = &ic; playbackTimerManager = &ptm; presenterCore = &pc; focusManager = &fm; setupListeners(); finaliseSetup();
#if !defined(JUCE_HEADLESS)
    playbackTimerManager->attachToVBlank(this);
#endif
}

void ControlPanel::setupViews() {
    waveformCanvasView = std::make_unique<WaveformCanvasView>(*this); addAndMakeVisible(waveformCanvasView.get());
    overlayView = std::make_unique<OverlayView>(*this); addAndMakeVisible(overlayView.get());
    topBarView = std::make_unique<TopBarView>(*this); addAndMakeVisible(topBarView.get());
    playbackTimeView = std::make_unique<PlaybackTimeView>(); addAndMakeVisible(playbackTimeView.get());
    fpsView = std::make_unique<FpsView>(); addAndMakeVisible(fpsView.get()); fpsView->toFront(false);
}

void ControlPanel::setupStrips() {
    inStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::In); addAndMakeVisible(inStrip.get());
    outStrip = std::make_unique<MarkerStrip>(MarkerStrip::MarkerType::Out); addAndMakeVisible(outStrip.get());
    cutLengthStrip = std::make_unique<CutLengthStrip>(); addAndMakeVisible(cutLengthStrip.get());
}

void ControlPanel::setupListeners() {
    playbackTimerManager->addListener(&getPresenterCore().getZoomPresenter());
    playbackTimerManager->addListener(&getPlaybackTextPresenter());
    playbackTimerManager->setZoomPointProvider([this]() {
        auto d = getMarkerMouseHandler().getDraggedHandle();
        return (d == MarkerMouseHandler::CutMarkerHandle::In) ? AppEnums::ActiveZoomPoint::In : (d == MarkerMouseHandler::CutMarkerHandle::Out) ? AppEnums::ActiveZoomPoint::Out : AppEnums::ActiveZoomPoint::None;
    });
    inStrip->setPresenter(&getBoundaryLogicPresenter()); outStrip->setPresenter(&getBoundaryLogicPresenter());
    addMouseListener(&getMarkerMouseHandler(), true); addMouseListener(&getWaveformMouseHandler(), true);
}

void ControlPanel::initialiseLookAndFeel() { setLookAndFeel(&modernLF); }
void ControlPanel::refreshThemeLive() { getPresenterCore().getThemePresenter().applyTheme(); }
void ControlPanel::finaliseSetup() { getPlaybackTextPresenter().initialiseEditors(); getPresenterCore().getControlStatePresenter().refreshStates(); getPresenterCore().getMatrixPresenter().fullMatrixUpdate(); refreshThemeLive(); }

void ControlPanel::resized() {
    if (layoutManager) { layoutManager->performLayout(); layoutManager->layoutWaveformArea(); }
    if (overlayView) overlayView->setBounds(getLocalBounds());
    if (fpsView && Config::Advanced::showFpsOverlay) {
        int x = (Config::Advanced::fpsOverlayPosition == Config::Advanced::posTopCenter || Config::Advanced::fpsOverlayPosition == Config::Advanced::posBottomCenter) ? (getWidth() - Config::Layout::Fps::width) / 2 : Config::Advanced::fpsOverlayX;
        int y = (Config::Advanced::fpsOverlayPosition == Config::Advanced::posTopCenter) ? Config::Layout::Fps::margin : (Config::Advanced::fpsOverlayPosition == Config::Advanced::posBottomCenter) ? getHeight() - Config::Layout::Fps::height - Config::Layout::Fps::margin : Config::Advanced::fpsOverlayY;
        fpsView->setBounds(x, y, Config::Layout::Fps::width, Config::Layout::Fps::height); fpsView->setVisible(true);
    } else if (fpsView) { fpsView->setVisible(false); }
}

void ControlPanel::paint(juce::Graphics &g) { g.fillAll(Config::Colors::Window::background); }
ControlPanel::~ControlPanel() { setLookAndFeel(nullptr); }

AudioPlayer &ControlPanel::getAudioPlayer() { return *owner.getAudioPlayer(); }
AudioPlayer &ControlPanel::getAudioPlayer() const { return *owner.getAudioPlayer(); }
const MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() const { return getPresenterCore().getCutPresenter().getMarkerMouseHandler(); }
MarkerMouseHandler &ControlPanel::getMarkerMouseHandler() { return getPresenterCore().getCutPresenter().getMarkerMouseHandler(); }
const WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() const { return getPresenterCore().getCutPresenter().getWaveformMouseHandler(); }
WaveformMouseHandler &ControlPanel::getWaveformMouseHandler() { return getPresenterCore().getCutPresenter().getWaveformMouseHandler(); }
juce::TextEditor &ControlPanel::getStatsDisplay() { return getPresenterCore().getStatsPresenter().getDisplay(); }
const juce::LookAndFeel &ControlPanel::getLookAndFeel() const { return modernLF; }
void ControlPanel::invokeOwnerOpenDialog() { owner.openButtonClicked(); }
juce::MouseCursor ControlPanel::getMouseCursor() { return (getInteractionCoordinator().getPlacementMode() != AppEnums::PlacementMode::None) ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::CrosshairCursor; }
