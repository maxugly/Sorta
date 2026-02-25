/**
 * @file PresenterCore.h
 * @audiofiler_full_context.txt
 * @ingroup Logic
 * @brief Aggregator for all presenters to reduce pointer bloat in ControlPanel.
 */

#ifndef AUDIOFILER_PRESENTERCORE_H
#define AUDIOFILER_PRESENTERCORE_H

#include <memory>

class ControlPanel;
class StatsPresenter;
class SilenceDetectionPresenter;
class PlaybackTextPresenter;
class RepeatButtonPresenter;
class BoundaryLogicPresenter;
class ControlButtonsPresenter;
class BoundaryLockPresenter;
class MarkerLockPresenter;
class CutButtonPresenter;
class CutResetPresenter;
class ControlStatePresenter;
class PlaybackRepeatController;
class MatrixPresenter;
class VolumePresenter;
class CutPresenter;
class SilenceThresholdPresenter;
class KeybindPresenter;
class FpsPresenter;
#include "Presenters/HintPresenter.h"
#include "Presenters/ZoomPresenter.h"

class PresenterCore final {
  public:
    explicit PresenterCore(ControlPanel &cp);
    ~PresenterCore();

    StatsPresenter& getStatsPresenter() { return *statsPresenter; }
    SilenceDetectionPresenter& getSilenceDetectionPresenter() { return *silenceDetectionPresenter; }
    PlaybackTextPresenter& getPlaybackTextPresenter() { return *playbackTextPresenter; }
    RepeatButtonPresenter& getRepeatButtonPresenter() { return *repeatButtonPresenter; }
    BoundaryLogicPresenter& getBoundaryLogicPresenter() { return *boundaryLogicPresenter; }
    BoundaryLockPresenter& getBoundaryLockPresenter() { return *boundaryLockPresenter; }
    MarkerLockPresenter& getMarkerLockPresenter() { return *markerLockPresenter; }
    ControlButtonsPresenter& getButtonPresenter() { return *buttonPresenter; }
    CutButtonPresenter& getCutButtonPresenter() { return *cutButtonPresenter; }
    CutResetPresenter& getCutResetPresenter() { return *cutResetPresenter; }
    ControlStatePresenter& getControlStatePresenter() { return *controlStatePresenter; }
    PlaybackRepeatController& getPlaybackRepeatController() { return *playbackRepeatController; }
    ZoomPresenter& getZoomPresenter() { return *zoomPresenter; }
    MatrixPresenter& getMatrixPresenter() { return *matrixPresenter; }
    VolumePresenter& getVolumePresenter() { return *volumePresenter; }
    CutPresenter& getCutPresenter() { return *cutPresenter; }
    const CutPresenter& getCutPresenter() const { return *cutPresenter; }
    SilenceThresholdPresenter& getSilenceThresholdPresenter() { return *silenceThresholdPresenter; }
    KeybindPresenter& getKeybindPresenter() { return *keybindPresenter; }
    FpsPresenter& getFpsPresenter() { return *fpsPresenter; }

  private:
    std::unique_ptr<StatsPresenter> statsPresenter;
    std::unique_ptr<SilenceDetectionPresenter> silenceDetectionPresenter;
    std::unique_ptr<PlaybackTextPresenter> playbackTextPresenter;
    std::unique_ptr<RepeatButtonPresenter> repeatButtonPresenter;
    std::unique_ptr<BoundaryLogicPresenter> boundaryLogicPresenter;
    std::unique_ptr<BoundaryLockPresenter> boundaryLockPresenter;
    std::unique_ptr<MarkerLockPresenter> markerLockPresenter;
    std::unique_ptr<ControlButtonsPresenter> buttonPresenter;
    std::unique_ptr<CutButtonPresenter> cutButtonPresenter;
    std::unique_ptr<CutResetPresenter> cutResetPresenter;
    std::unique_ptr<ControlStatePresenter> controlStatePresenter;
    std::unique_ptr<PlaybackRepeatController> playbackRepeatController;
    std::unique_ptr<ZoomPresenter> zoomPresenter;
    std::unique_ptr<MatrixPresenter> matrixPresenter;
    std::unique_ptr<VolumePresenter> volumePresenter;
    std::unique_ptr<CutPresenter> cutPresenter;
    std::unique_ptr<SilenceThresholdPresenter> silenceThresholdPresenter;
    std::unique_ptr<KeybindPresenter> keybindPresenter;
    std::unique_ptr<HintPresenter> hintPresenter;
    std::unique_ptr<FpsPresenter> fpsPresenter;

    ControlPanel &owner;

    void setupPresenters();
};

#endif
