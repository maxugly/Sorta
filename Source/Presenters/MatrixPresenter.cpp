
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
    auto& silenceDetector = owner.getSilenceDetector();
    auto* sdp = owner.getSilenceDetectionPresenter();

    // 1. audioPlayer.getAudioFormatReader() != nullptr (Active Stream Open)
    state.ledColors.push_back(audioPlayer.getAudioFormatReader() != nullptr ? active : inactive);

    // 2. RAM Cache Hit
    state.ledColors.push_back(sessionState.hasMetadataForFile(sessionState.getCurrentFilePath()) ? active : inactive);

    // 3. sdp && sdp->isAnalyzing() (Background Thread Busy)
    state.ledColors.push_back((sdp && sdp->isAnalyzing()) ? active : inactive);

    // 4. interactionCoordinator.getNeedsJumpToCutIn() (Async Audio Jump Pending)
    state.ledColors.push_back(interactionCoordinator.getNeedsJumpToCutIn() ? active : inactive);

    // 5. audioPlayer.isPlaying() (Transport Active)
    state.ledColors.push_back(audioPlayer.isPlaying() ? active : inactive);

    // 6. audioPlayer.isRepeating() (Repeat Logic Armed)
    state.ledColors.push_back(audioPlayer.isRepeating() ? active : inactive);

    // 7. sessionState.getCutPrefs().autoplay (AutoPlay Armed)
    state.ledColors.push_back(sessionState.getCutPrefs().autoplay ? active : inactive);

    // 8. sessionState.getCutPrefs().active (Cut Logic Active)
    state.ledColors.push_back(sessionState.getCutPrefs().active ? active : inactive);

    // 9. sessionState.getCutPrefs().autoCut.inActive (Ghost Auto-In Armed)
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.inActive ? active : inactive);

    // 10. sessionState.getCutPrefs().autoCut.outActive (Ghost Auto-Out Armed)
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.outActive ? active : inactive);

    // 11. audioPlayer.getThumbnail().getTotalLength() > 0.0 (Buffer Loaded)
    state.ledColors.push_back(audioPlayer.getThumbnail().getTotalLength() > 0.0 ? active : inactive);

    // 12. audioPlayer.getCurrentPosition() > 0.0 (Playhead Advanced)
    state.ledColors.push_back(audioPlayer.getCurrentPosition() > 0.0 ? active : inactive);

    // 13. interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutIn (Placement Mode In)
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutIn ? active : inactive);

    // 14. interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutOut (Placement Mode Out)
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutOut ? active : inactive);

    // 15. interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None (Zooming)
    state.ledColors.push_back(interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None ? active : inactive);

    // 16. interactionCoordinator.shouldShowEyeCandy() (Heavy Rendering Mode)
    state.ledColors.push_back(interactionCoordinator.shouldShowEyeCandy() ? active : inactive);

    // 17. waveformMouseHandler.isScrubbing() (User Scrubbing)
    state.ledColors.push_back(waveformMouseHandler.isScrubbing() ? active : inactive);

    // 18. waveformMouseHandler.isDragging() (User Dragging)
    state.ledColors.push_back(waveformMouseHandler.isDragging() ? active : inactive);

    // 19. markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None (Marker Dragging)
    state.ledColors.push_back(markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);

    // 20. markerMouseHandler.getHoveredHandle() != MarkerMouseHandler::CutMarkerHandle::None (Marker Hovering)
    state.ledColors.push_back(markerMouseHandler.getHoveredHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);

    // 21. timerManager.isZKeyDown() (Hardware Key Lock)
    state.ledColors.push_back(timerManager.isZKeyDown() ? active : inactive);

    // 22. sessionState.getViewMode() == AppEnums::ViewMode::Overlay (Overlay View)
    state.ledColors.push_back(sessionState.getViewMode() == AppEnums::ViewMode::Overlay ? active : inactive);

    // 23. sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo (Stereo Render Engine)
    state.ledColors.push_back(sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo ? active : inactive);

    // 24-26. PlaybackTimeView editors
    auto* ptv = owner.getPlaybackTimeView();
    state.ledColors.push_back((ptv && ptv->getElapsedEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((ptv && ptv->getRemainingEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((ptv && ptv->getCutLengthEditor().hasKeyboardFocus(false)) ? active : inactive);

    // 27-28. MarkerStrip editors
    auto* inStrip = owner.getInStrip();
    auto* outStrip = owner.getOutStrip();
    state.ledColors.push_back((inStrip && inStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((outStrip && outStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);

    // 29-30. SilenceDetector threshold editors
    state.ledColors.push_back(silenceDetector.getInSilenceThresholdEditor().hasKeyboardFocus(false) ? active : inactive);
    state.ledColors.push_back(silenceDetector.getOutSilenceThresholdEditor().hasKeyboardFocus(false) ? active : inactive);

    // 31. Constant true flag
    state.ledColors.push_back(active);

    // 32. Heartbeat
    state.ledColors.push_back(Config::Colors::Matrix::ledPulse.withAlpha(timerManager.getBreathingPulse()));

    matrixView.updateState(state);
}

void MatrixPresenter::animationUpdate(float) {
}
