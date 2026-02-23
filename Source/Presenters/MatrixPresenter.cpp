
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

MatrixPresenter::MatrixPresenter(ControlPanel& cp)
    : owner(cp) {
    owner.getPlaybackTimerManager().addListener(this);
}

MatrixPresenter::~MatrixPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void MatrixPresenter::playbackTimerTick() {
    auto& matrixView = owner.getMatrixView();

    MatrixViewState state;
    const auto active = Config::Colors::Matrix::ledActive;
    const auto inactive = Config::Colors::Matrix::ledInactive;

    auto& audioPlayer = owner.getAudioPlayer();
    auto& sessionState = owner.getSessionState();
    auto& interactionCoordinator = owner.getInteractionCoordinator();
    auto& waveformMouseHandler = owner.getWaveformMouseHandler();
    auto& markerMouseHandler = owner.getMarkerMouseHandler();
    auto& timerManager = owner.getPlaybackTimerManager();
    auto* sdp = owner.getSilenceDetectionPresenter();

    // 1. audioPlayer.isPlaying()
    state.ledColors.push_back(audioPlayer.isPlaying() ? active : inactive);

    // 2. audioPlayer.isRepeating()
    state.ledColors.push_back(audioPlayer.isRepeating() ? active : inactive);

    // 3. sessionState.getCutPrefs().autoplay
    state.ledColors.push_back(sessionState.getCutPrefs().autoplay ? active : inactive);

    // 4. sessionState.getCutPrefs().active (Cut Mode)
    state.ledColors.push_back(sessionState.getCutPrefs().active ? active : inactive);

    // 5. sessionState.getCutPrefs().autoCut.inActive
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.inActive ? active : inactive);

    // 6. sessionState.getCutPrefs().autoCut.outActive
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.outActive ? active : inactive);

    // 7. silenceDetectionPresenter->isAnalyzing()
    state.ledColors.push_back((sdp && sdp->isAnalyzing()) ? active : inactive);

    // 8. interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutIn
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutIn ? active : inactive);

    // 9. interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutOut
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutOut ? active : inactive);

    // 10. interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None
    state.ledColors.push_back(interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None ? active : inactive);

    // 11. interactionCoordinator.shouldShowEyeCandy()
    state.ledColors.push_back(interactionCoordinator.shouldShowEyeCandy() ? active : inactive);

    // 12. interactionCoordinator.getNeedsJumpToCutIn()
    state.ledColors.push_back(interactionCoordinator.getNeedsJumpToCutIn() ? active : inactive);

    // 13. waveformMouseHandler.isScrubbing()
    state.ledColors.push_back(waveformMouseHandler.isScrubbing() ? active : inactive);

    // 14. waveformMouseHandler.isDragging()
    state.ledColors.push_back(waveformMouseHandler.isDragging() ? active : inactive);

    // 15. markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None
    state.ledColors.push_back(markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);

    // 16. playbackTimerManager.isZKeyDown()
    state.ledColors.push_back(timerManager.isZKeyDown() ? active : inactive);

    // 17. sessionState.getViewMode() == AppEnums::ViewMode::Overlay
    state.ledColors.push_back(sessionState.getViewMode() == AppEnums::ViewMode::Overlay ? active : inactive);

    // 18. sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo
    state.ledColors.push_back(sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo ? active : inactive);

    // 19. audioPlayer.getThumbnail().getTotalLength() > 0.0
    state.ledColors.push_back(audioPlayer.getThumbnail().getTotalLength() > 0.0 ? active : inactive);

    // 20. audioPlayer.getCurrentPosition() > 0.0
    state.ledColors.push_back(audioPlayer.getCurrentPosition() > 0.0 ? active : inactive);

    // 21. Heartbeat
    state.ledColors.push_back(Config::Colors::Matrix::ledPulse.withAlpha(timerManager.getBreathingPulse()));

    matrixView.updateState(state);
}

void MatrixPresenter::animationUpdate(float) {
}
