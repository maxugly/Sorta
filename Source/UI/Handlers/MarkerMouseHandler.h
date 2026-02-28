#ifndef AUDIOFILER_MARKERMOUSEHANDLER_H
#define AUDIOFILER_MARKERMOUSEHANDLER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file MarkerMouseHandler.h
 * @ingroup UI
 * @brief Handles mouse interactions specific to cut markers (In, Out, and Full Region).
 */

class ControlPanel;

/**
 * @class MarkerMouseHandler
 * @brief Handles low-level mouse interactions for audio cut markers.
 * 
 * @details Architecturally, MarkerMouseHandler serves as a specialized 
 *          interaction handler that decouples marker-specific mouse event 
 *          processing from the main CutLayerView. It follows the principle 
 *          of keeping Views "dumb" by encapsulating the logic for dragging 
 *          individual 'In'/'Out' points or moving the entire cut region 
 *          simultaneously. It interacts with the InteractionCoordinator 
 *          to validate positions and ensure snapping rules are applied. 
 *          By managing transient states like which handle is currently 
 *          hovered or dragged, it allows the View to focus purely on 
 *          rendering the marker assets.
 * 
 * @see ControlPanel, InteractionCoordinator, WaveformMouseHandler, CutPresenter
 */
class MarkerMouseHandler : public juce::MouseListener {
  public:
    /** 
     * @enum CutMarkerHandle
     * @brief Identifies which part of the cut marker system is being interacted with.
     */
    enum class CutMarkerHandle { 
        None, /**< No handle selected. */
        In,   /**< The 'In' point marker handle. */
        Out,  /**< The 'Out' point marker handle. */
        Full  /**< The entire region between markers. */
    };

    /**
     * @brief Constructs a new MarkerMouseHandler.
     * @param controlPanel The parent ControlPanel shell.
     */
    explicit MarkerMouseHandler(ControlPanel &controlPanel);

    /** @brief Destructor. */
    ~MarkerMouseHandler() override = default;

    /** @brief Handles mouse movement, updating hover states for marker handles. */
    void mouseMove(const juce::MouseEvent &event) override;

    /** @brief Handles mouse down events, initiating marker drags or region moves. */
    void mouseDown(const juce::MouseEvent &event) override;

    /** @brief Handles mouse drag events, updating marker positions in real-time. */
    void mouseDrag(const juce::MouseEvent &event) override;

    /** @brief Handles mouse up events, finalizing the marker movement. */
    void mouseUp(const juce::MouseEvent &event) override;

    /** @brief Handles mouse exit from the marker area, clearing hover states. */
    void mouseExit(const juce::MouseEvent &event) override;

    /** @return The handle currently under the mouse cursor. */
    CutMarkerHandle getHoveredHandle() const { return hoveredHandle; }

    /** @return The handle currently being dragged by the user. */
    CutMarkerHandle getDraggedHandle() const { return draggedHandle; }

    /** 
     * @brief Returns true if the given handle is currently hovered, dragged, or armed. 
     * @param handle The handle to check.
     */
    bool isHandleActive(CutMarkerHandle handle) const;

    /** 
     * @brief Programmatically sets the currently dragged handle. 
     * @param handle The handle to set as being dragged.
     */
    void setDraggedHandle(CutMarkerHandle handle) { draggedHandle = handle; }

  private:
    /** @brief Internal helper to update the hoveredHandle state based on mouse position. */
    void refreshHoverState(const juce::MouseEvent &event);

    /** 
     * @brief Determines which marker handle is at a given pixel position. 
     * @param pos The pixel position to check.
     * @return The handle at that position, or None.
     */
    CutMarkerHandle getHandleAtPosition(juce::Point<int> pos) const;

    /** 
     * @brief Translates a pixel X coordinate into a time in seconds. 
     * @param x The pixel coordinate.
     * @param bounds The drawing bounds.
     * @param duration The total duration of the audio.
     * @return The calculated time in seconds.
     */
    double getMouseTime(int x, const juce::Rectangle<int> &bounds, double duration) const;

    ControlPanel &owner;
    CutMarkerHandle hoveredHandle = CutMarkerHandle::None;
    CutMarkerHandle draggedHandle = CutMarkerHandle::None;
    double dragStartCutLength = 0.0;
    double dragStartMouseOffset = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerMouseHandler)
};

#endif
