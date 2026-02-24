#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Components/TransportButton.h"

class CutLengthStrip : public juce::Component {
public:
    CutLengthStrip();
    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::TextEditor& getLengthEditor() { return lengthEditor; }
    TransportButton& getLockButton() { return lockButton; }
private:
    juce::TextEditor lengthEditor;
    TransportButton lockButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLengthStrip)
};
