#ifndef AUDIOFILER_FOCUSMANAGER_H
#define AUDIOFILER_FOCUSMANAGER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file FocusManager.h
 * @ingroup UI
 * @brief Manager for handling UI focus and keyboard interaction targets.
 */

class ControlPanel;

/** 
 * @enum FocusTarget
 * @brief Identifies the current target of user interaction or keyboard focus.
 */
enum class FocusTarget {
    None,        /**< No specific focus target. */
    CutIn,       /**< Focus is on the 'In' marker or text editor. */
    CutOut,      /**< Focus is on the 'Out' marker or text editor. */
    Playback,    /**< Focus is on the current playback playhead. */
    MouseManual  /**< Focus is determined by manual mouse positioning. */
};

/**
 * @class FocusManager
 * @brief Orchestrates the focus state and interaction logic for UI components.
 * 
 * @details Architecturally, the FocusManager serves as a specialized Manager that 
 *          tracks which application component currently holds the "active focus" 
 *          for interaction. Following the principle of keeping Views "dumb," 
 *          it centralizes the logic for determining which time-point in the 
 *          audio session is currently the subject of user operations (e.g., 
 *          zooming or scrubbing). By abstracting focus state, it allows 
 *          Presenters to query a single source of truth for contextual 
 *          behavior without needing to know the specific UI component that 
 *          triggered the focus change.
 * 
 * @see ControlPanel, InteractionCoordinator, KeybindPresenter
 */
class FocusManager {
  public:
    /**
     * @brief Constructs a new FocusManager.
     * @param owner The parent ControlPanel shell.
     */
    explicit FocusManager(ControlPanel &owner);

    /** @return The currently active focus target. */
    FocusTarget getCurrentTarget() const;

    /** @return The time in seconds associated with the current focus target. */
    double getFocusedTime() const;

    /** 
     * @brief Calculates a step multiplier for movements based on modifier keys. 
     * @param shift True if the Shift key is held.
     * @param ctrl True if the Ctrl key is held.
     * @return A multiplier (e.g., 10x for shift, 0.1x for ctrl).
     */
    static double getStepMultiplier(bool shift, bool ctrl);

  private:
    ControlPanel &owner;
};

#endif
