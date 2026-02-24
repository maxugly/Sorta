#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class HintView : public juce::Component {
public:
    HintView();
    void setHint(const juce::String& text);
    void paint(juce::Graphics& g) override;
private:
    juce::String currentHint;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HintView)
};
