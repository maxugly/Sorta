#pragma once
#include <JuceHeader.h>

class StyledTextEditor : public juce::TextEditor {
public:
    StyledTextEditor();
    void applyStandardStyle(juce::Justification justification = juce::Justification::centred);
    void setJustification(juce::Justification newJustification) {
        justification = newJustification;
        juce::TextEditor::setJustification(newJustification);
    }
    juce::Justification getJustification() const { return justification; }

    void setCustomTextColor(juce::Colour newColor);
    juce::Colour getCustomTextColor() const;

private:
    juce::Justification justification = juce::Justification::centred;
};
