#ifndef AUDIOFILER_PRESENTERCORE_H
#define AUDIOFILER_PRESENTERCORE_H

#include <memory>

class ControlPanel;
class StatsPresenter;
class SilenceDetectionPresenter;
class PlaybackTextPresenter;
class CutLengthPresenter;
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
class ThemePresenter;
#include "Presenters/HintPresenter.h"
#include "Presenters/ZoomPresenter.h"

/**
 * @file PresenterCore.h
 * @ingroup Logic
 * @brief The central aggregator and lifecycle manager for all UI Presenters.
 * 
 * @details Architecturally, PresenterCore serves as a "Dependency Container" 
 *          that eliminates pointer bloat within the `ControlPanel`. It acts 
 *          as the orchestration layer, responsible for constructing, 
 *          initializing, and wiring all individual Presenters to their 
 *          respective Views and the global SessionState.
 * 
 *          Following the Model-View-Presenter (MVP) pattern, this class 
 *          ensures that the "Glue" logic (the Presenters) has a single, 
 *          controlled point of origin, preventing scattered initialization 
 *          and memory leaks. It enforces the "Lego Standard" by treating 
 *          each feature presenter as a self-contained brick owned by this core.
 * 
 * @see ControlPanel
 * @see SessionState
 * @see PresenterCore
 */
class PresenterCore final {
  public:
    /**
     * @brief Constructs the core and triggers the instantiation of all sub-presenters.
     * @param cp Reference to the parent ControlPanel (the View shell).
     */
    explicit PresenterCore(ControlPanel &cp);

    /**
     * @brief Ensures all presenters are cleanly deallocated in the correct order.
     */
    ~PresenterCore();

    /** @name Presenter Accessors
     *  Provides type-safe access to individual feature presenters.
     */
    /**@{*/
    StatsPresenter& getStatsPresenter() { return *statsPresenter; }
    SilenceDetectionPresenter& getSilenceDetectionPresenter() { return *silenceDetectionPresenter; }
    PlaybackTextPresenter& getPlaybackTextPresenter() { return *playbackTextPresenter; }
    CutLengthPresenter& getCutLengthPresenter() { return *cutLengthPresenter; }
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
    ThemePresenter& getThemePresenter() { return *themePresenter; }
    /**@}*/

  private:
    std::unique_ptr<StatsPresenter> statsPresenter;               /**< Logic for the metadata display. */
    std::unique_ptr<SilenceDetectionPresenter> silenceDetectionPresenter; /**< Logic for the silence scanning status. */
    std::unique_ptr<PlaybackTextPresenter> playbackTextPresenter; /**< Logic for the primary playhead timer. */
    std::unique_ptr<CutLengthPresenter> cutLengthPresenter;       /**< Logic for the region duration display. */
    std::unique_ptr<RepeatButtonPresenter> repeatButtonPresenter; /**< Logic for the loop toggle. */
    std::unique_ptr<BoundaryLogicPresenter> boundaryLogicPresenter; /**< Logic for 'In'/'Out' boundary interaction. */
    std::unique_ptr<BoundaryLockPresenter> boundaryLockPresenter; /**< Logic for individual boundary locks. */
    std::unique_ptr<MarkerLockPresenter> markerLockPresenter;     /**< Logic for the relative length lock. */
    std::unique_ptr<ControlButtonsPresenter> buttonPresenter;     /**< Logic for transport controls (Play/Stop). */
    std::unique_ptr<CutButtonPresenter> cutButtonPresenter;       /**< Logic for the 'Cut Mode' toggle. */
    std::unique_ptr<CutResetPresenter> cutResetPresenter;         /**< Logic for resetting markers. */
    std::unique_ptr<ControlStatePresenter> controlStatePresenter; /**< Logic for global application mode states. */
    std::unique_ptr<PlaybackRepeatController> playbackRepeatController; /**< Logic for automated repeat behavior. */
    std::unique_ptr<ZoomPresenter> zoomPresenter;                 /**< Logic for horizontal waveform scaling. */
    std::unique_ptr<MatrixPresenter> matrixPresenter;             /**< Logic for the auxiliary LED matrix view. */
    std::unique_ptr<VolumePresenter> volumePresenter;             /**< Logic for the master gain control. */
    std::unique_ptr<CutPresenter> cutPresenter;                   /**< Logic for visual boundary synchronization. */
    std::unique_ptr<SilenceThresholdPresenter> silenceThresholdPresenter; /**< Logic for dB threshold adjustment. */
    std::unique_ptr<KeybindPresenter> keybindPresenter;           /**< Logic for keyboard interaction mapping. */
    std::unique_ptr<HintPresenter> hintPresenter;                 /**< Logic for the interactive status bar. */
    std::unique_ptr<FpsPresenter> fpsPresenter;                   /**< Logic for performance monitoring display. */
    std::unique_ptr<ThemePresenter> themePresenter;               /**< Logic for the palette switching engine. */

    ControlPanel &owner;                                          /**< The root View container (MVP View). */

    /**
     * @brief Internal helper to allocate and configure all presenters.
     * @details Called by the constructor to ensure atomic initialization of the core.
     */
    void setupPresenters();
};

#endif
