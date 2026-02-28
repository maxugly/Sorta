#ifndef AUDIOFILER_ZOOMPRESENTER_H
#define AUDIOFILER_ZOOMPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "Core/AppEnums.h"

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

/**
 * @file ZoomPresenter.h
 * @ingroup Logic
 * @brief Coordinates horizontal waveform scaling and zoom popup visualization.
 * 
 * @details Architecturally, ZoomPresenter acts as the "Presenter Glue" that 
 *          manages the complex scaling logic for the application's waveform 
 *          rendering. It follows the MVP Law by translating user scroll and 
 *          keybind interactions into normalized zoom factors in `SessionState`.
 * 
 *          Key responsibilities:
 *          - **Real-time Zoom Tracking**: Monitors the `PlaybackTimerManager` 
 *            to update the "Zoom HUD" and the secondary "Zoom Popup" preview.
 *          - **Contextual Scaling**: Calculates the optimal zoom range based 
 *            on the currently hovered time segment or active playback point.
 *          - **Boundary Synchronization**: Ensures that as the user zooms in, 
 *            the active marker or playhead remains centered and visually stable.
 * 
 * @see ZoomView
 * @see SessionState
 * @see InteractionCoordinator
 * @see CoordinateMapper
 */
class ZoomPresenter final : public PlaybackTimerManager::Listener {
  public:
    /**
     * @brief Constructs the presenter and wires it to the parent view shell.
     * @param owner Reference to the main ControlPanel.
     */
    explicit ZoomPresenter(ControlPanel& owner);

    /**
     * @brief Unregisters from global timer and state notifications.
     */
    ~ZoomPresenter() override;

    /**
     * @brief Periodic callback for updating high-frequency zoom visuals.
     * @details Used to refresh the dynamic bounds of the zoom preview popup 
     *          at a consistent frame rate.
     */
    void playbackTimerTick() override;

    /**
     * @brief Reacts to changes in the primary focus point for zooming.
     * @param newPoint The new focal point (In, Out, or Playback).
     */
    void activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) override;

  private:
    ControlPanel& owner;        /**< Reference to the host View shell. */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomPresenter)
};

#endif
