
#include "Presenters/MatrixPresenter.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/InteractionCoordinator.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "Workers/SilenceDetector.h"
#include "Utils/Config.h"

MatrixPresenter::MatrixPresenter(ControlPanel& cp, AudioPlayer& player, SessionState& state,
                                 SilenceDetector& detector, InteractionCoordinator& coordinator,
                                 MatrixView& view)
    : owner(cp), audioPlayer(player), sessionState(state), silenceDetector(detector),
      interactionCoordinator(coordinator), matrixView(view) {
    owner.getPlaybackTimerManager().addListener(this);
}

MatrixPresenter::~MatrixPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void MatrixPresenter::playbackTimerTick() {
    MatrixViewState state;
    const auto active = Config::Colors::Matrix::ledActive;
    const auto inactive = Config::Colors::Matrix::ledInactive;

    // 1. audioPlayer.isPlaying()
    state.ledColors.push_back(audioPlayer.isPlaying() ? active : inactive);

    // 2. silenceDetectionPresenter.isAnalyzing()
    bool isAnalyzing = false;
    if (auto* sdp = owner.getSilenceDetectionPresenter())
        isAnalyzing = sdp->isAnalyzing();
    state.ledColors.push_back(isAnalyzing ? active : inactive);

    // 3. audioPlayer.getThumbnail().getTotalLength() > 0.0
    state.ledColors.push_back(audioPlayer.getThumbnail().getTotalLength() > 0.0 ? active : inactive);

    // 4. sessionState.getCutPrefs().active
    state.ledColors.push_back(sessionState.getCutPrefs().active ? active : inactive);

    // 5. sessionState.getCutPrefs().autoCut.inActive || sessionState.getCutPrefs().autoCut.outActive
    const auto& cutPrefs = sessionState.getCutPrefs();
    state.ledColors.push_back((cutPrefs.autoCut.inActive || cutPrefs.autoCut.outActive) ? active : inactive);

    // 6. waveformMouseHandler.isScrubbing() || markerMouseHandler.getDraggedHandle() != None
    bool isInteracting = owner.getWaveformMouseHandler().isScrubbing() || 
                         owner.getMarkerMouseHandler().getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None;
    state.ledColors.push_back(isInteracting ? active : inactive);

    // 7. interactionCoordinator.getPlacementMode() != None || interactionCoordinator.getActiveZoomPoint() != None
    bool zoomOrPlacement = (interactionCoordinator.getPlacementMode() != AppEnums::PlacementMode::None) ||
                           (interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None);
    state.ledColors.push_back(zoomOrPlacement ? active : inactive);

    // 8. Heartbeat
    float pulse = owner.getPlaybackTimerManager().getBreathingPulse();
    state.ledColors.push_back(Config::Colors::Matrix::ledPulse.withAlpha(pulse));

    matrixView.updateState(state);
}

void MatrixPresenter::animationUpdate(float) {
}
