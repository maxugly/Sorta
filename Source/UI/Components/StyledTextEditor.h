#pragma once
#include <JuceHeader.h>

class StyledTextEditor : public juce::TextEditor {
public:
    StyledTextEditor();
    void applyStandardStyle(juce::Justification justification = juce::Justification::centred);
};
