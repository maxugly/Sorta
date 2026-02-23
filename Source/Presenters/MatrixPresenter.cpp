
#include "Presenters/MatrixPresenter.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/InteractionCoordinator.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Views/TopBarView.h"
#include "UI/Views/PlaybackTimeView.h"
#include "UI/Components/MarkerStrip.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/PresenterCore.h"
#include "Core/WaveformManager.h"
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

    // 1-31: Existing logic
    state.ledColors.push_back(audioPlayer.getAudioFormatReader() != nullptr ? active : inactive);
    state.ledColors.push_back(sessionState.hasMetadataForFile(sessionState.getCurrentFilePath()) ? active : inactive);
    state.ledColors.push_back((sdp && sdp->isAnalyzing()) ? active : inactive);
    state.ledColors.push_back(interactionCoordinator.getNeedsJumpToCutIn() ? active : inactive);
    state.ledColors.push_back(audioPlayer.isPlaying() ? active : inactive);
    state.ledColors.push_back(audioPlayer.isRepeating() ? active : inactive);
    state.ledColors.push_back(sessionState.getCutPrefs().autoplay ? active : inactive);
    state.ledColors.push_back(sessionState.getCutPrefs().active ? active : inactive);
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.inActive ? active : inactive);
    state.ledColors.push_back(sessionState.getCutPrefs().autoCut.outActive ? active : inactive);
    state.ledColors.push_back(audioPlayer.getThumbnail().getTotalLength() > 0.0 ? active : inactive);
    state.ledColors.push_back(audioPlayer.getCurrentPosition() > 0.0 ? active : inactive);
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutIn ? active : inactive);
    state.ledColors.push_back(interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::CutOut ? active : inactive);
    state.ledColors.push_back(interactionCoordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None ? active : inactive);
    state.ledColors.push_back(interactionCoordinator.shouldShowEyeCandy() ? active : inactive);
    state.ledColors.push_back(waveformMouseHandler.isScrubbing() ? active : inactive);
    state.ledColors.push_back(waveformMouseHandler.isDragging() ? active : inactive);
    state.ledColors.push_back(markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);
    state.ledColors.push_back(markerMouseHandler.getHoveredHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);
    state.ledColors.push_back(timerManager.isZKeyDown() ? active : inactive);
    state.ledColors.push_back(sessionState.getViewMode() == AppEnums::ViewMode::Overlay ? active : inactive);
    state.ledColors.push_back(sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo ? active : inactive);

    auto* ptv = owner.getPlaybackTimeView();
    state.ledColors.push_back((ptv && ptv->getElapsedEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((ptv && ptv->getRemainingEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((ptv && ptv->getCutLengthEditor().hasKeyboardFocus(false)) ? active : inactive);

    auto* inStrip = owner.getInStrip();
    auto* outStrip = owner.getOutStrip();
    state.ledColors.push_back((inStrip && inStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((outStrip && outStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);

    state.ledColors.push_back(silenceDetector.getInSilenceThresholdEditor().hasKeyboardFocus(false) ? active : inactive);
    state.ledColors.push_back(silenceDetector.getOutSilenceThresholdEditor().hasKeyboardFocus(false) ? active : inactive);
    state.ledColors.push_back(active); // Light 31 (Original Flag)

    // 32. Anchor
    state.ledColors.push_back(active);

    // 33. Stats Overlay Active
    state.ledColors.push_back(owner.getPresenterCore().getStatsPresenter().isShowingStats() ? active : inactive);

    // 34. Dragging full cut region
    state.ledColors.push_back(markerMouseHandler.isHandleActive(MarkerMouseHandler::CutMarkerHandle::Full) ? active : inactive);

    // 35. Mouse inside Waveform
    state.ledColors.push_back(waveformMouseHandler.getMouseCursorX() != -1 ? active : inactive);

    // 36. File Fully Analyzed
    state.ledColors.push_back(sessionState.getCurrentMetadata().isAnalyzed ? active : inactive);

    // 37. Playhead inside Cut Region
    double pos = audioPlayer.getCurrentPosition();
    state.ledColors.push_back((pos >= sessionState.getCutIn() && pos <= sessionState.getCutOut()) ? active : inactive);

    // 38. Non-default Zoom Level
    state.ledColors.push_back(sessionState.getZoomFactor() != 10.0f ? active : inactive);

    // 39. Cut-In Modified
    state.ledColors.push_back(sessionState.getCutIn() > 0.0 ? active : inactive);

    // 40. Cut-Out Modified
    state.ledColors.push_back((sessionState.getCutOut() > 0.0 && sessionState.getCutOut() < sessionState.getTotalDuration()) ? active : inactive);

    // 41. Interaction started in Zoom popup
    state.ledColors.push_back(waveformMouseHandler.wasInteractionInZoom() ? active : inactive);

    // 42. Manual Zoom Override
    state.ledColors.push_back(interactionCoordinator.getManualZoomPoint() != AppEnums::ActiveZoomPoint::None ? active : inactive);

    // 43. Stereo File Loaded
    state.ledColors.push_back(audioPlayer.getWaveformManager().getThumbnail().getNumChannels() > 1 ? active : inactive);

    // 44. Top Bar Hovered
    state.ledColors.push_back((owner.getTopBarView() && owner.getTopBarView()->isMouseOver(true)) ? active : inactive);

    // 45. Playback Time View Hovered
    state.ledColors.push_back((owner.getPlaybackTimeView() && owner.getPlaybackTimeView()->isMouseOver(true)) ? active : inactive);

    // 46. In Strip Hovered
    state.ledColors.push_back((owner.getInStrip() && owner.getInStrip()->isMouseOver(true)) ? active : inactive);

    // 47. Out Strip Hovered
    state.ledColors.push_back((owner.getOutStrip() && owner.getOutStrip()->isMouseOver(true)) ? active : inactive);

    // 48. Heartbeat
    state.ledColors.push_back(Config::Colors::Matrix::ledPulse.withAlpha(timerManager.getBreathingPulse()));

    matrixView.updateState(state);
}

void MatrixPresenter::animationUpdate(float) {
}
