#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class CutLengthStrip : public juce::Component {
public:
    CutLengthStrip();
    void resized() override;
    juce::TextEditor& getLengthEditor() { return lengthEditor; }
private:
    juce::TextEditor lengthEditor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLengthStrip)
};
