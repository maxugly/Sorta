#ifndef AUDIOFILER_CUTMARKERSVIEW_H
#define AUDIOFILER_CUTMARKERSVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Views/CutLayerState.h"

/**
 * @class CutMarkersView
 * @brief Renders the vertical cut markers and selected region outline.
 */
class CutMarkersView : public juce::Component {
  public:
    CutMarkersView();
    ~CutMarkersView() override;

    void paint(juce::Graphics &g) override;
    void updateState(const CutLayerState& newState);

  private:
    CutLayerState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutMarkersView)
};

#endif
