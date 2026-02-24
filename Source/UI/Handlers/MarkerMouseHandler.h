/**
 * @file MarkerMouseHandler.h
 * @audiofiler_full_context.txt
 * @ingroup UI
 * @brief Handles mouse interactions specific to cut markers.
 */

#ifndef AUDIOFILER_MARKERMOUSEHANDLER_H
#define AUDIOFILER_MARKERMOUSEHANDLER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

class MarkerMouseHandler : public juce::MouseListener {
  public:
    enum class CutMarkerHandle { None, In, Out, Full };

    explicit MarkerMouseHandler(ControlPanel &controlPanel);
    ~MarkerMouseHandler() override = default;

    void mouseMove(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;

    CutMarkerHandle getHoveredHandle() const { return hoveredHandle; }
    CutMarkerHandle getDraggedHandle() const { return draggedHandle; }

    /** @brief Returns true if the given handle is currently hovered, dragged, or armed. */
    bool isHandleActive(CutMarkerHandle handle) const;

    void setDraggedHandle(CutMarkerHandle handle) { draggedHandle = handle; }

  private:
    CutMarkerHandle getHandleAtPosition(juce::Point<int> pos) const;
    double getMouseTime(int x, const juce::Rectangle<int> &bounds, double duration) const;

    ControlPanel &owner;
    CutMarkerHandle hoveredHandle = CutMarkerHandle::None;
    CutMarkerHandle draggedHandle = CutMarkerHandle::None;
    double dragStartCutLength = 0.0;
    double dragStartMouseOffset = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerMouseHandler)
};

#endif
