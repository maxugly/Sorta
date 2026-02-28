#pragma once

#include "Core/SessionState.h"
#include "UI/Views/VolumeView.h"

/**
 * @file VolumePresenter.h
 * @ingroup Logic
 * @brief Presenter responsible for master volume logic and UI synchronization.
 * 
 * @details Architecturally, VolumePresenter acts as the "Presenter Glue" between 
 *          the `SessionState` (Model) and the `VolumeView` (View). It follows 
 *          the MVP Law by ensuring that changes to the master gain in the 
 *          Model are instantly reflected in the visual state of the volume dial, 
 *          and vice-versa.
 * 
 *          Key responsibilities:
 *          - **State Observation**: Monitors `SessionState::volumeChanged` to 
 *            update the `VolumeView` when the volume is adjusted via keybinds 
 *            or external commands.
 *          - **User Input Handling**: Directs updates from the `VolumeView` 
 *            (e.g., dial turns) back into the `SessionState`.
 * 
 * @see VolumeView
 * @see SessionState
 * @see AudioPlayer
 */
class VolumePresenter final : public SessionState::Listener {
  public:
    /**
     * @brief Constructs the presenter and wires the view to the model.
     * @param v Reference to the VolumeView (passive UI component).
     * @param s Reference to the SessionState (the brain).
     */
    VolumePresenter(VolumeView &v, SessionState &s);

    /**
     * @brief Unregisters as a listener to prevent dangling callbacks.
     */
    ~VolumePresenter() override;

    /**
     * @brief Reacts to global volume changes.
     * @param newVolume The new gain level (0.0 to 1.0).
     */
    void volumeChanged(float newVolume) override;

  private:
    VolumeView &view;               /**< The passive View component. */
    SessionState &sessionState;      /**< The central application Model. */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumePresenter)
};
