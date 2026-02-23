

#include "Presenters/CutPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/CutLayerView.h"
#include "Workers/SilenceDetector.h"
#include "UI/InteractionCoordinator.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Core/WaveformManager.h"

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
    state.cutInSeconds = sessionState.getCutIn();
    state.cutOutSeconds = sessionState.getCutOut();
    state.audioLength = cutLayerView.getOwner().getAudioPlayer().getThumbnail().getTotalLength();
    state.inThreshold = silenceDetector.getCurrentInSilenceThreshold();
    state.outThreshold = silenceDetector.getCurrentOutSilenceThreshold();
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
