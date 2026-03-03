#ifndef AUDIOFILER_CUTTHRESHOLDSVIEW_H
#define AUDIOFILER_CUTTHRESHOLDSVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Views/CutLayerState.h"

/**
 * @class CutThresholdsView
 * @brief Renders the horizontal silence threshold lines.
 */
class CutThresholdsView : public juce::Component {
  public:
    CutThresholdsView();
    ~CutThresholdsView() override;

    void paint(juce::Graphics &g) override;
    void updateState(const CutLayerState& newState);

  private:
    CutLayerState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutThresholdsView)
};

#endif
