#ifndef AUDIOFILER_LAYOUTMANAGER_H
#define AUDIOFILER_LAYOUTMANAGER_H

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class LayoutManager
 * @brief Handles the positioning and sizing of all child components within the ControlPanel.
 */
class LayoutManager final
{
public:
    explicit LayoutManager(ControlPanel& controlPanelIn);

    /**
     * @brief Recomputes every sub-layout (top/bottom rows, cut controls, waveform, stats).
     */
    void performLayout();

private:
    void layoutTopRowButtons(juce::Rectangle<int>& bounds, int rowHeight);
    void layoutCutControls(juce::Rectangle<int>& bounds, int rowHeight);
    void layoutBottomRowAndTextDisplay(juce::Rectangle<int>& bounds, int rowHeight);
    void layoutWaveformAndStats(juce::Rectangle<int>& bounds);

    ControlPanel& controlPanel;
};

#endif // AUDIOFILER_LAYOUTMANAGER_H
