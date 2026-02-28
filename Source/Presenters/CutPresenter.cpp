

#include "Presenters/CutPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/CutLayerView.h"
#include "UI/InteractionCoordinator.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Core/WaveformManager.h"
#include "Utils/CoordinateMapper.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/PlaybackCursorView.h"

CutPresenter::CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                           WaveformView &waveformViewIn,
                           CutLayerView &cutLayerViewIn,
                           PlaybackCursorView &playbackCursorViewIn,
                           InteractionCoordinator &interactionCoordinatorIn,
                           PlaybackTimerManager &playbackTimerManagerIn)
    : sessionState(sessionStateIn), 
      waveformView(waveformViewIn),
      cutLayerView(cutLayerViewIn),
      playbackCursorView(playbackCursorViewIn),
      interactionCoordinator(interactionCoordinatorIn),
      playbackTimerManager(playbackTimerManagerIn) {
    markerMouseHandler = std::make_unique<MarkerMouseHandler>(controlPanel);
    waveformMouseHandler = std::make_unique<WaveformMouseHandler>(controlPanel);
    sessionState.addListener(this);
    controlPanel.getAudioPlayer().getWaveformManager().addChangeListener(this);
    playbackTimerManager.addListener(this);

    refreshMarkersVisibility();
    pushStateToView();
}

CutPresenter::~CutPresenter() {
    playbackTimerManager.removeListener(this);
    cutLayerView.getOwner().getAudioPlayer().getWaveformManager().removeChangeListener(this);
    sessionState.removeListener(this);
}

void CutPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &) {
    refreshMarkersVisibility();
    pushStateToView();
}

void CutPresenter::playbackTimerTick() {
    pushStateToView();
}

void CutPresenter::updateAnimationState(CutLayerState& state) {
    auto invertCol = [](juce::Colour c) {
        return juce::Colour((juce::uint8)(255 - c.getRed()),
                            (juce::uint8)(255 - c.getGreen()),
                            (juce::uint8)(255 - c.getBlue()));
    };

    // Also update marker interaction states high-frequency
    auto calcMarkerProps = [&](MarkerMouseHandler::CutMarkerHandle handle, bool isAuto) {
        juce::Colour color = isAuto ? Config::Colors::cutMarkerAuto : Config::Colors::cutLine;
        float thickness = Config::Layout::Glow::cutBoxOutlineThickness;
        bool pulse = false;

        const auto dragged = markerMouseHandler->getDraggedHandle();
        const auto hovered = markerMouseHandler->getHoveredHandle();

        if (dragged != MarkerMouseHandler::CutMarkerHandle::None) {
            if (dragged == handle) {
                color = invertCol(color);
                thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
            }
        } else if (hovered == handle) {
            color = invertCol(color);
            thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        }

        return std::make_tuple(color, thickness, pulse);
    };

    const auto draggedHandle = markerMouseHandler->getDraggedHandle();
    const auto hoveredHandle = markerMouseHandler->getHoveredHandle();

    auto inProps = calcMarkerProps(MarkerMouseHandler::CutMarkerHandle::In, sessionState.getCutPrefs().autoCut.inActive);
    state.inMarkerColor = std::get<0>(inProps);
    state.inMarkerThickness = std::get<1>(inProps);
    state.inMarkerShouldPulse = std::get<2>(inProps);
    
    // Threshold lines follow the same inversion logic as markers
    state.inThresholdColor = Config::Colors::thresholdLine;
    if (hoveredHandle == MarkerMouseHandler::CutMarkerHandle::In || draggedHandle == MarkerMouseHandler::CutMarkerHandle::In)
        state.inThresholdColor = invertCol(state.inThresholdColor);

    auto outProps = calcMarkerProps(MarkerMouseHandler::CutMarkerHandle::Out, sessionState.getCutPrefs().autoCut.outActive);
    state.outMarkerColor = std::get<0>(outProps);
    state.outMarkerThickness = std::get<1>(outProps);
    state.outMarkerShouldPulse = std::get<2>(outProps);

    state.outThresholdColor = Config::Colors::thresholdLine;
    if (hoveredHandle == MarkerMouseHandler::CutMarkerHandle::Out || draggedHandle == MarkerMouseHandler::CutMarkerHandle::Out)
        state.outThresholdColor = invertCol(state.outThresholdColor);

    state.regionOutlineColor = Config::Colors::cutLine;
    state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThickness;
    state.regionShouldPulse = false;

    if (draggedHandle == MarkerMouseHandler::CutMarkerHandle::Full) {
        state.regionOutlineColor = invertCol(state.regionOutlineColor);
        state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    } else if (draggedHandle == MarkerMouseHandler::CutMarkerHandle::None && hoveredHandle == MarkerMouseHandler::CutMarkerHandle::Full) {
        state.regionOutlineColor = invertCol(state.regionOutlineColor);
        state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    }

    cutLayerView.updateState(state);

    // --- Waveform State Logic ---
    WaveformViewState waveformState;
    waveformState.thumbnail = &cutLayerView.getOwner().getAudioPlayer().getWaveformManager().getThumbnail();
    waveformState.totalLength = waveformState.thumbnail->getTotalLength();
    waveformState.channelMode = cutLayerView.getOwner().getChannelViewMode();
    waveformView.updateState(waveformState);

    // --- Playhead State Logic ---
    PlaybackCursorViewState cursorState;
    const double audioLength = waveformState.totalLength;
    if (audioLength > 0.0) {
        const auto &layout = cutLayerView.getOwner().getWaveformBounds();
        cursorState.playheadX = CoordinateMapper::secondsToPixels(
            cutLayerView.getOwner().getAudioPlayer().getCurrentPosition(),
            (float)layout.getWidth(), audioLength);

        const auto activePoint = interactionCoordinator.getActiveZoomPoint();
        const bool isZooming = playbackTimerManager.isZKeyDown() || activePoint != AppEnums::ActiveZoomPoint::None;

        if (isZooming && interactionCoordinator.getZoomPopupBounds()
                             .translated(-layout.getX(), -layout.getY())
                             .contains(juce::roundToInt(cursorState.playheadX), 10)) {
            cursorState.isVisible = false;
        } else {
            cursorState.isVisible = true;
        }

        const auto qColor = Config::Colors::quaternary;
        cursorState.centerLineColor = juce::Colour((juce::uint8)(255 - qColor.getRed()),
                                                   (juce::uint8)(255 - qColor.getGreen()),
                                                   (juce::uint8)(255 - qColor.getBlue()));
        cursorState.headColor = Config::Colors::playbackCursorHead;
    } else {
        cursorState.isVisible = false;
    }
    playbackCursorView.updateState(cursorState);
}

void CutPresenter::pushStateToView() {
    CutLayerState state;
    const double cutIn = sessionState.getCutIn();
    const double cutOut = sessionState.getCutOut();
    const double audioLength = cutLayerView.getOwner().getAudioPlayer().getThumbnail().getTotalLength();
    const auto bounds = cutLayerView.getLocalBounds();
    const float viewWidth = (float)bounds.getWidth();

    state.inPixelX = (float)bounds.getX() + CoordinateMapper::secondsToPixels(cutIn, viewWidth, audioLength);
    state.outPixelX = (float)bounds.getX() + CoordinateMapper::secondsToPixels(cutOut, viewWidth, audioLength);

    const float inX = juce::jlimit((float)bounds.getX(), (float)bounds.getRight(), state.inPixelX);
    const float outX = juce::jlimit((float)bounds.getX(), (float)bounds.getRight(), state.outPixelX);

    state.actualInX = juce::jmin(inX, outX);
    state.actualOutX = juce::jmax(inX, outX);

    auto calcThresholdY = [&](float threshold) {
        const float centerY = (float)bounds.getCentreY();
        const float halfHeight = (float)bounds.getHeight() / 2.0f;
        float top = centerY - (threshold * halfHeight);
        float bottom = centerY + (threshold * halfHeight);
        return std::make_pair(juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), top),
                              juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), bottom));
    };

    auto inY = calcThresholdY(sessionState.getCutPrefs().autoCut.thresholdIn);
    state.inThresholdYTop = inY.first;
    state.inThresholdYBottom = inY.second;

    auto outY = calcThresholdY(sessionState.getCutPrefs().autoCut.thresholdOut);
    state.outThresholdYTop = outY.first;
    state.outThresholdYBottom = outY.second;

    state.audioLength = (float)audioLength;
    state.markersVisible = sessionState.getCutPrefs().active;
    state.channelMode = cutLayerView.getOwner().getChannelViewMode();

    updateAnimationState(state);
}

void CutPresenter::refreshMarkersVisibility() {
    // This is now handled via pushStateToView and state.markersVisible
}

void CutPresenter::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &cutLayerView.getOwner().getAudioPlayer().getWaveformManager().getThumbnail()) {
        waveformView.clearCaches();
    }
}
