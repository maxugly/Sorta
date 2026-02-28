#ifndef AUDIOFILER_WAVEFORMMOUSEHANDLER_H
#define AUDIOFILER_WAVEFORMMOUSEHANDLER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file WaveformMouseHandler.h
 * @ingroup UI
 * @brief Handles mouse interactions specific to the waveform (scrubbing, zooming, seek).
 */

class ControlPanel;

/**
 * @class WaveformMouseHandler
 * @brief Handles low-level mouse interactions for the waveform display.
 * 
 * @details Architecturally, WaveformMouseHandler serves as a specialized 
 *          interaction handler that decouples mouse event processing from 
 *          the main WaveformView. It follows the principle of keeping Views 
 *          "dumb" by encapsulating the logic for scrubbing, seeking, and 
 *          right-click context actions. It interacts with the 
 *          InteractionCoordinator and SessionState to translate raw mouse 
 *          coordinates into time-based operations. By managing transient 
 *          states like scrubbing or drag start positions, it allows the 
 *          View to focus purely on rendering the audio data.
 * 
 * @see ControlPanel, InteractionCoordinator, MarkerMouseHandler, SessionState
 */
class WaveformMouseHandler : public juce::MouseListener {
  public:
    /**
     * @brief Constructs a new WaveformMouseHandler.
     * @param controlPanel The parent ControlPanel shell.
     */
    explicit WaveformMouseHandler(ControlPanel &controlPanel);

    /** @brief Destructor. */
    ~WaveformMouseHandler() override = default;

    /** @brief Handles mouse movement over the waveform, updating hover states and HUDs. */
    void mouseMove(const juce::MouseEvent &event) override;

    /** @brief Handles mouse down events, initiating seeks or scrubs. */
    void mouseDown(const juce::MouseEvent &event) override;

    /** @brief Handles mouse drag events for continuous scrubbing or selection. */
    void mouseDrag(const juce::MouseEvent &event) override;

    /** @brief Handles mouse up events, finalizing interactions. */
    void mouseUp(const juce::MouseEvent &event) override;

    /** @brief Handles mouse exit from the waveform area, clearing hover states. */
    void mouseExit(const juce::MouseEvent &event) override;

    /** 
     * @brief Handles mouse wheel movement for horizontal zooming. 
     * @details Delegates the complex zoom math to the InteractionCoordinator.
     */
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    /** @return The current horizontal mouse cursor position in pixels. */
    int getMouseCursorX() const { return mouseCursorX; }

    /** @return The current vertical mouse cursor position in pixels. */
    int getMouseCursorY() const { return mouseCursorY; }

    /** @return The current time in seconds under the mouse cursor. */
    double getMouseCursorTime() const { return mouseCursorTime; }

    /** @return True if the user is currently performing a scrubbing operation. */
    bool isScrubbing() const { return isScrubbingState; }

    /** @return True if a mouse drag is currently in progress. */
    bool isDragging() const { return isDraggingFlag; }

    /** @return True if the current interaction originated within a zoom popup. */
    bool wasInteractionInZoom() const { return interactionStartedInZoom; }

  private:
    /** 
     * @brief Translates a pixel X coordinate into a time in seconds. 
     * @param x The pixel coordinate.
     * @param bounds The drawing bounds of the waveform.
     * @param duration The total duration of the audio.
     * @return The calculated time in seconds.
     */
    double getMouseTime(int x, const juce::Rectangle<int> &bounds, double duration) const;

    /** 
     * @brief Processes a right-click to place a cut marker. 
     * @param x The pixel X coordinate of the click.
     */
    void handleRightClickForCutPlacement(int x);

    /** 
     * @brief Seeks the audio playback to the specified pixel position. 
     * @param x The pixel X coordinate to seek to.
     */
    void seekToMousePosition(int x);

    /** @brief Clears focus from any active text editors if the click warrants it. */
    void clearTextEditorFocusIfNeeded(const juce::MouseEvent &event);

    ControlPanel &owner;
    int mouseCursorX = -1, mouseCursorY = -1;
    double mouseCursorTime = 0.0;
    bool isDraggingFlag = false;
    int mouseDragStartX = 0;
    bool interactionStartedInZoom = false;
    bool isScrubbingState = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformMouseHandler)
};

#endif
