
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
#include "Utils/Config.h"
#include "UI/FocusManager.h"
#include "UI/Views/VolumeView.h"

MatrixPresenter::MatrixPresenter(ControlPanel& cp)
    : owner(cp) {
    owner.getPlaybackTimerManager().addListener(this);
    owner.getSessionState().addListener(this);
    owner.getInteractionCoordinator().addListener(this);
    owner.getAudioPlayer().addChangeListener(this);
    // Removed immediate fullMatrixUpdate() to prevent crash during presenter construction
}

MatrixPresenter::~MatrixPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
    owner.getSessionState().removeListener(this);
    owner.getInteractionCoordinator().removeListener(this);
    owner.getAudioPlayer().removeChangeListener(this);
}

void MatrixPresenter::fullMatrixUpdate() {
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
    state.ledColors.push_back(inactive); // Eye Candy removed
    state.ledColors.push_back(waveformMouseHandler.isScrubbing() ? active : inactive);
    state.ledColors.push_back(waveformMouseHandler.isDragging() ? active : inactive);
    state.ledColors.push_back(markerMouseHandler.getDraggedHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);
    state.ledColors.push_back(markerMouseHandler.getHoveredHandle() != MarkerMouseHandler::CutMarkerHandle::None ? active : inactive);
    state.ledColors.push_back(timerManager.isZKeyDown() ? active : inactive);
    state.ledColors.push_back(sessionState.getViewMode() == AppEnums::ViewMode::Overlay ? active : inactive);
    state.ledColors.push_back(sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Stereo ? active : inactive);

    auto* ptv = owner.getPlaybackTimeView();
    auto* cls = owner.getCutLengthStrip();
    state.ledColors.push_back((ptv && ptv->getElapsedEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((ptv && ptv->getRemainingEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((cls && cls->getLengthEditor().hasKeyboardFocus(false)) ? active : inactive);

    auto* inStrip = owner.getInStrip();
    auto* outStrip = owner.getOutStrip();
    state.ledColors.push_back((inStrip && inStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((outStrip && outStrip->getTimerEditor().hasKeyboardFocus(false)) ? active : inactive);

    state.ledColors.push_back((inStrip && inStrip->getThresholdEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back((outStrip && outStrip->getThresholdEditor().hasKeyboardFocus(false)) ? active : inactive);
    state.ledColors.push_back(active); // Light 31 (Original Flag)

    // 32. Volume Flame Indicator
    float vol = sessionState.getVolume();
    juce::Colour flameColor;
    if (vol < 0.33f) {
        flameColor = Config::Colors::VolumeFlame::low.interpolatedWith(Config::Colors::VolumeFlame::mid, vol / 0.33f);
    } else if (vol < 0.66f) {
        flameColor = Config::Colors::VolumeFlame::mid.interpolatedWith(Config::Colors::VolumeFlame::high, (vol - 0.33f) / 0.33f);
    } else {
        flameColor = Config::Colors::VolumeFlame::high.interpolatedWith(Config::Colors::VolumeFlame::peak, (vol - 0.66f) / 0.34f);
    }
    state.ledColors.push_back(flameColor);

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

    // 48. Worker Scanning Forward
    state.ledColors.push_back((sdp && sdp->isAnalyzingIn()) ? active : inactive);

    // 49. Worker Scanning Backward
    state.ledColors.push_back((sdp && sdp->isAnalyzingOut()) ? active : inactive);

    // 50. Focus Engine: Cut In
    state.ledColors.push_back(owner.getFocusManager().getCurrentTarget() == FocusTarget::CutIn ? active : inactive);

    // 51. Focus Engine: Cut Out
    state.ledColors.push_back(owner.getFocusManager().getCurrentTarget() == FocusTarget::CutOut ? active : inactive);

    // 52. Focus Engine: Playback
    state.ledColors.push_back(owner.getFocusManager().getCurrentTarget() == FocusTarget::Playback ? active : inactive);

    // 53. Focus Engine: Mouse Override
    state.ledColors.push_back(owner.getFocusManager().getCurrentTarget() == FocusTarget::MouseManual ? active : inactive);

    // 54. Hardware: Shift
    state.ledColors.push_back(juce::ModifierKeys::getCurrentModifiers().isShiftDown() ? active : inactive);

    // 55. Hardware: Ctrl
    state.ledColors.push_back(juce::ModifierKeys::getCurrentModifiers().isCtrlDown() ? active : inactive);

    // 56. Hardware: Alt
    state.ledColors.push_back(juce::ModifierKeys::getCurrentModifiers().isAltDown() ? active : inactive);

    // 57. Classic UI Mode
    state.ledColors.push_back(sessionState.getViewMode() == AppEnums::ViewMode::Classic ? active : inactive);

    // 58. Mono Render Mode
    state.ledColors.push_back(sessionState.getChannelViewMode() == AppEnums::ChannelViewMode::Mono ? active : inactive);

    // 59. Volume Knob Hovered
    state.ledColors.push_back((owner.getTopBarView() && owner.getTopBarView()->getVolumeView().isMouseOver(true)) ? active : inactive);

    // 60. Matrix Itself Hovered
    state.ledColors.push_back(matrixView.isMouseOver(true) ? active : inactive);

    // 61. Audio RAM Buffer Full
    state.ledColors.push_back(audioPlayer.getThumbnail().isFullyLoaded() ? active : inactive);

    // 62. Playhead at Absolute Zero
    state.ledColors.push_back(audioPlayer.getCurrentPosition() == 0.0 ? active : inactive);

    // 63. Entire File Selected
    state.ledColors.push_back((sessionState.getCutIn() == 0.0 && sessionState.getCutOut() >= sessionState.getTotalDuration() && sessionState.getTotalDuration() > 0.0) ? active : inactive);

    // 64. Heartbeat removed
    state.ledColors.push_back(inactive);

    cachedState = state;
    matrixView.updateState(cachedState);
}

void MatrixPresenter::playbackTimerTick() {
    // Intentionally empty. Observer Law: No polling for state changes.
}
