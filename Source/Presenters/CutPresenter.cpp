

#include "Presenters/CutPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/CutLayerView.h"
#include "Workers/SilenceDetector.h"
#include "UI/InteractionCoordinator.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Core/WaveformManager.h"
#include "Utils/CoordinateMapper.h"

CutPresenter::CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                           CutLayerView &cutLayerViewIn, SilenceDetector &silenceDetectorIn,
                           InteractionCoordinator &interactionCoordinatorIn,
                           PlaybackTimerManager &playbackTimerManagerIn)
    : sessionState(sessionStateIn), cutLayerView(cutLayerViewIn),
      silenceDetector(silenceDetectorIn), interactionCoordinator(interactionCoordinatorIn),
      playbackTimerManager(playbackTimerManagerIn) {
    markerMouseHandler = std::make_unique<MarkerMouseHandler>(controlPanel);
    waveformMouseHandler = std::make_unique<WaveformMouseHandler>(controlPanel);
    sessionState.addListener(this);
    playbackTimerManager.addListener(this);

    refreshMarkersVisibility();
    pushStateToView();
}

CutPresenter::~CutPresenter() {
    playbackTimerManager.removeListener(this);
    sessionState.removeListener(this);
}

void CutPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &) {
    refreshMarkersVisibility();
    pushStateToView();
}

void CutPresenter::playbackTimerTick() {
    pushStateToView();
}

void CutPresenter::animationUpdate(float) {
    pushStateToView();
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

    auto calcThresholdY = [&](float threshold) {
        const float centerY = (float)bounds.getCentreY();
        const float halfHeight = (float)bounds.getHeight() / 2.0f;
        float top = centerY - (threshold * halfHeight);
        float bottom = centerY + (threshold * halfHeight);
        return std::make_pair(juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), top),
                              juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), bottom));
    };

    auto inY = calcThresholdY(silenceDetector.getCurrentInSilenceThreshold());
    state.inThresholdYTop = inY.first;
    state.inThresholdYBottom = inY.second;

    auto outY = calcThresholdY(silenceDetector.getCurrentOutSilenceThreshold());
    state.outThresholdYTop = outY.first;
    state.outThresholdYBottom = outY.second;

    state.fadeWidthPixels = viewWidth * Config::Layout::Waveform::cutRegionFadeProportion;
    state.audioLength = (float)audioLength;
    state.glowAlpha = playbackTimerManager.getBreathingPulse();
    state.showEyeCandy = interactionCoordinator.shouldShowEyeCandy();
    state.isAutoIn = silenceDetector.getIsAutoCutInActive();
    state.isAutoOut = silenceDetector.getIsAutoCutOutActive();
    state.markersVisible = sessionState.getCutPrefs().active;
    state.draggedHandle = markerMouseHandler->getDraggedHandle();
    state.hoveredHandle = markerMouseHandler->getHoveredHandle();
    state.channelMode = cutLayerView.getOwner().getChannelViewMode();

    cutLayerView.updateState(state);
}

void CutPresenter::refreshMarkersVisibility() {
    // This is now handled via pushStateToView and state.markersVisible
}
