#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Components/TransportButton.h"

#include "UI/Components/StyledTextEditor.h"

class CutLengthStrip : public juce::Component {
public:
    CutLengthStrip();
    void paint(juce::Graphics& g) override;
    void resized() override;
    StyledTextEditor& getLengthEditor() { return lengthEditor; }
    TransportButton& getLockButton() { return lockButton; }
private:
    StyledTextEditor lengthEditor;
    TransportButton lockButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLengthStrip)
};
