#ifndef AUDIOFILER_LAYOUTMANAGER_H
#define AUDIOFILER_LAYOUTMANAGER_H

#include <JuceHeader.h>

/**
 * @file LayoutManager.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Manager responsible for calculating and applying UI component bounds.
 */

class ControlPanel;

/**
 * @class LayoutManager
 * @brief Handles the complex calculation and application of UI component bounds within the ControlPanel.
 * 
 * @details Architecturally, the LayoutManager is a "Dumb Component" or utility that 
 *          encapsulates the visual organization of the ControlPanel. Following the 
 *          Model-View-Presenter (MVP) law, it contains no business logic or state; 
 *          it exists purely to keep the ControlPanel's resized() method clean and 
 *          maintainable. It relies entirely on the ControlPanel's structure to 
 *          distribute space among child views, ensuring a consistent and responsive 
 *          user interface.
 * 
 * @see ControlPanel
 */
class LayoutManager final {
  public:
    /**
     * @brief Constructs a new LayoutManager.
     * @param controlPanelIn Reference to the parent ControlPanel to be laid out.
     */
    explicit LayoutManager(ControlPanel &controlPanelIn);

    /** 
     * @brief Orchestrates the full layout pass for the ControlPanel. 
     * @details This method is typically called from ControlPanel::resized().
     */
    void performLayout();

    /** @brief Lays out the waveform area with horizontal and vertical splits. */
    void layoutWaveformArea();

  private:
    /** 
     * @brief Lays out the top row of buttons (Open, Settings, etc.). 
     * @param bounds The available drawing area, updated after allocation.
     * @param rowHeight The preferred height for this row.
     */
    void layoutTopRowButtons(juce::Rectangle<int> &bounds, int rowHeight);

    /** 
     * @brief Lays out the cut markers and associated text editors. 
     * @param bounds The available drawing area, updated after allocation.
     * @param rowHeight The preferred height for these controls.
     */
    void layoutCutControls(juce::Rectangle<int> &bounds, int rowHeight);

    /** 
     * @brief Lays out the transport controls and playback text display. 
     * @param bounds The available drawing area, updated after allocation.
     * @param rowHeight The preferred height for this row.
     */
    void layoutBottomRowAndTextDisplay(juce::Rectangle<int> &bounds, int rowHeight);

    /** 
     * @brief Lays out the primary waveform area and any overlay stats. 
     * @param bounds The remaining available drawing area.
     */
    void layoutWaveformAndStats(juce::Rectangle<int> &bounds);

    ControlPanel &controlPanel;
};

#endif
