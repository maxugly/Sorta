#ifndef AUDIOFILER_CUTLAYERVIEW_H
#define AUDIOFILER_CUTLAYERVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <memory>

#include "UI/Views/CutLayerState.h"

/**
 * @file CutLayerView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief View component responsible for rendering cut markers, thresholds, and regions.
 */

class ControlPanel;
class CutFadeView;
class CutThresholdsView;
class CutMarkersView;

/**
 * @class CutLayerView
 * @brief A passive UI component that renders the cut markers, silence thresholds, and selected regions.
 * 
 * @details Architecturally, the CutLayerView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the visual elements associated 
 *          with audio cutting (markers, thresholds, region highlights). It 
 *          relies entirely on the CutPresenter to calculate its visual state 
 *          (via CutLayerState) and push it through the updateState() method.
 * 
 * @see CutPresenter, WaveformCanvasView, CutLayerState
 */
class CutLayerView : public juce::Component {
  public:
    /**
     * @brief Constructs a new CutLayerView.
     * @param owner Reference to the parent ControlPanel.
     */
    explicit CutLayerView(ControlPanel &owner);

    /** @brief Destructor. */
    ~CutLayerView() override;

    /** 
     * @brief Updates the view's internal state and triggers a repaint. 
     * @param newState The new visual state to apply.
     */
    void updateState(const CutLayerState& newState);

    /** @return Const reference to the current visual state. */
    const CutLayerState& getCurrentState() const { return state; }

    /** @return Reference to the parent ControlPanel. */
    ControlPanel& getOwner() { return owner; }

    void resized() override;

  private:
    ControlPanel &owner;
    CutLayerState state;
    std::unique_ptr<CutFadeView> fadeView;
    std::unique_ptr<CutThresholdsView> thresholdsView;
    std::unique_ptr<CutMarkersView> markersView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLayerView)
};

#endif
