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
class CutButtonPresenter;
class CutResetPresenter;
class ControlStatePresenter;
class PlaybackRepeatController;
class MatrixPresenter;
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
    ControlButtonsPresenter& getButtonPresenter() { return *buttonPresenter; }
    CutButtonPresenter& getCutButtonPresenter() { return *cutButtonPresenter; }
    CutResetPresenter& getCutResetPresenter() { return *cutResetPresenter; }
    ControlStatePresenter& getControlStatePresenter() { return *controlStatePresenter; }
    PlaybackRepeatController& getPlaybackRepeatController() { return *playbackRepeatController; }
    ZoomPresenter& getZoomPresenter() { return *zoomPresenter; }
    MatrixPresenter& getMatrixPresenter() { return *matrixPresenter; }

  private:
    std::unique_ptr<StatsPresenter> statsPresenter;
    std::unique_ptr<SilenceDetectionPresenter> silenceDetectionPresenter;
    std::unique_ptr<PlaybackTextPresenter> playbackTextPresenter;
    std::unique_ptr<RepeatButtonPresenter> repeatButtonPresenter;
    std::unique_ptr<BoundaryLogicPresenter> boundaryLogicPresenter;
    std::unique_ptr<ControlButtonsPresenter> buttonPresenter;
    std::unique_ptr<CutButtonPresenter> cutButtonPresenter;
    std::unique_ptr<CutResetPresenter> cutResetPresenter;
    std::unique_ptr<ControlStatePresenter> controlStatePresenter;
    std::unique_ptr<PlaybackRepeatController> playbackRepeatController;
    std::unique_ptr<ZoomPresenter> zoomPresenter;
    std::unique_ptr<MatrixPresenter> matrixPresenter;

    ControlPanel &owner;

    void setupPresenters();
};

#endif
