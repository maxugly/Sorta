#ifndef AUDIOFILER_CUTFADEVIEW_H
#define AUDIOFILER_CUTFADEVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Views/CutLayerState.h"

/**
 * @class CutFadeView
 * @brief Renders the fade/cut regions that darken audio outside the active selection.
 */
class CutFadeView : public juce::Component {
  public:
    CutFadeView();
    ~CutFadeView() override;

    void paint(juce::Graphics &g) override;
    void updateState(const CutLayerState& newState);

  private:
    CutLayerState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutFadeView)
};

#endif
