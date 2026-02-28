#ifndef AUDIOFILER_INTERACTIONCOORDINATOR_H
#define AUDIOFILER_INTERACTIONCOORDINATOR_H

#include "Core/AppEnums.h"
#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif
#include <utility>

/**
 * @file InteractionCoordinator.h
 * @ingroup UI
 * @brief Coordinator for transient UI interaction states and cross-component logic.
 */

/**
 * @class InteractionCoordinator
 * @brief Manages transient UI interaction states that are not part of the global SessionState.
 *
 * @details Architecturally, the InteractionCoordinator serves as a specialized Manager that 
 *          handles highly volatile UI states, such as which marker is currently being 
 *          hovered or the temporary bounds of a zoom popup. Following the principle 
 *          of keeping Views "dumb," the InteractionCoordinator centralizes complex 
 *          interaction logic—like snapping, marker validation, and mouse-wheel zoom 
 *          math—that would otherwise clutter individual Presenters or Views. It 
 *          acts as a neutral intermediary, allowing different UI layers to synchronize 
 *          without directly depending on each other's internal state.
 * 
 * @see SessionState, ControlPanel, WaveformMouseHandler, MarkerMouseHandler
 */
class InteractionCoordinator {
  public:
    /** 
     * @class Listener
     * @brief Listener interface for receiving updates about interaction state changes.
     */
    class Listener {
      public:
        virtual ~Listener() = default;

        /** 
         * @brief Triggered when the current placement mode (In/Out/None) changes. 
         * @param newMode The new active placement mode.
         */
        virtual void placementModeChanged(AppEnums::PlacementMode newMode) = 0;
    };

    /** @brief Default constructor. */
    InteractionCoordinator() = default;

    /** @brief Destructor. */
    ~InteractionCoordinator() = default;

    /** @brief Adds a listener to be notified of state changes. */
    void addListener(Listener *l);

    /** @brief Removes a previously registered listener. */
    void removeListener(Listener *l);

    /** @brief Sets the currently active zoom point (In, Out, or None). */
    void setActiveZoomPoint(AppEnums::ActiveZoomPoint point) {
        m_activeZoomPoint = point;
    }

    /** @brief Returns the currently active zoom point. */
    AppEnums::ActiveZoomPoint getActiveZoomPoint() const {
        return m_activeZoomPoint;
    }

    /** @brief Sets a manual zoom point override (e.g. from mouse hover). */
    void setManualZoomPoint(AppEnums::ActiveZoomPoint point) {
        m_manualZoomPoint = point;
    }

    /** @brief Returns the manual zoom point override. */
    AppEnums::ActiveZoomPoint getManualZoomPoint() const {
        return m_manualZoomPoint;
    }

    /** @brief Sets whether a jump to cut-in is pending. */
    void setNeedsJumpToCutIn(bool needs) {
        m_needsJumpToCutIn = needs;
    }

    /** @brief Returns true if a jump to cut-in is pending. */
    bool getNeedsJumpToCutIn() const {
        return m_needsJumpToCutIn;
    }

    /** @brief Sets the current zoom popup bounds. */
    void setZoomPopupBounds(juce::Rectangle<int> bounds) {
        m_zoomPopupBounds = bounds;
    }

    /** @brief Returns the current zoom popup bounds. */
    juce::Rectangle<int> getZoomPopupBounds() const {
        return m_zoomPopupBounds;
    }

    /** @brief Sets the current zoom time range. */
    void setZoomTimeRange(double start, double end) {
        m_zoomTimeRange = {start, end};
    }

    /** @brief Returns the current zoom time range. */
    std::pair<double, double> getZoomTimeRange() const {
        return m_zoomTimeRange;
    }

    /** @brief Returns the current placement mode. */
    AppEnums::PlacementMode getPlacementMode() const {
        return m_placementMode;
    }

    /** 
     * @brief Sets the current placement mode and notifies listeners. 
     * @param mode The new placement mode.
     */
    void setPlacementMode(AppEnums::PlacementMode mode) {
        m_placementMode = mode;
        listeners.call(&Listener::placementModeChanged, m_placementMode);
    }

    /** 
     * @brief Snaps a raw time to relevant boundaries (e.g. zero-crossings or samples). 
     * @param rawTime The un-snapped time in seconds.
     * @param sampleRate The sample rate of the current audio file.
     * @return The snapped time in seconds.
     */
    double getSnappedTime(double rawTime, double sampleRate) const;

    /** 
     * @brief Validates and constraints a marker position based on project rules. 
     * @param marker The marker being moved.
     * @param newPosition Input/Output parameter for the position.
     * @param cutIn The current 'In' point.
     * @param cutOut The current 'Out' point.
     * @param duration The total duration of the audio.
     */
    void validateMarkerPosition(AppEnums::ActiveZoomPoint marker, double &newPosition, double cutIn,
                                double cutOut, double duration) const;

    /** 
     * @brief Handles the logic for moving a full cut region. 
     * @param newIn Input/Output parameter for the 'In' point.
     * @param newOut Input/Output parameter for the 'Out' point.
     * @param length The current length of the cut region.
     * @param duration The total duration of the audio.
     */
    void constrainFullRegionMove(double &newIn, double &newOut, double length,
                                 double duration) const;

    /** 
     * @brief Handles mouse-wheel zoom logic by updating SessionState. 
     * @param wheel The mouse wheel event details.
     * @param state The SessionState to update.
     */
    void handleMouseWheelZoom(const juce::MouseWheelDetails &wheel, class SessionState &state) const;

  private:
    juce::ListenerList<Listener> listeners;
    AppEnums::ActiveZoomPoint m_activeZoomPoint = AppEnums::ActiveZoomPoint::None;
    AppEnums::ActiveZoomPoint m_manualZoomPoint = AppEnums::ActiveZoomPoint::None;
    bool m_needsJumpToCutIn = false;
    juce::Rectangle<int> m_zoomPopupBounds;
    std::pair<double, double> m_zoomTimeRange;
    AppEnums::PlacementMode m_placementMode = AppEnums::PlacementMode::None;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractionCoordinator)
};

#endif
