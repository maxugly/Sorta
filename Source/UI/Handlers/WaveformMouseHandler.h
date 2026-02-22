/**
 * @file WaveformMouseHandler.h
 * @audiofiler_full_context.txt
 * @ingroup UI
 * @brief Handles mouse interactions specific to the waveform (scrubbing, zooming, seek).
 */

#ifndef AUDIOFILER_WAVEFORMMOUSEHANDLER_H
#define AUDIOFILER_WAVEFORMMOUSEHANDLER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

class WaveformMouseHandler : public juce::MouseListener {
  public:
    explicit WaveformMouseHandler(ControlPanel &controlPanel);
    ~WaveformMouseHandler() override = default;

    void mouseMove(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    int getMouseCursorX() const { return mouseCursorX; }
    int getMouseCursorY() const { return mouseCursorY; }
    double getMouseCursorTime() const { return mouseCursorTime; }

    bool isScrubbing() const { return isScrubbingState; }
    bool isDragging() const { return isDraggingFlag; }
    bool wasInteractionInZoom() const { return interactionStartedInZoom; }

  private:
    double getMouseTime(int x, const juce::Rectangle<int> &bounds, double duration) const;
    void handleRightClickForCutPlacement(int x);
    void seekToMousePosition(int x);
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
