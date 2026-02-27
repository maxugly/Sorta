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
    void paint(juce::Graphics& g) override;

    void focusGained(FocusChangeType cause) override;
    void focusLost(FocusChangeType cause) override;

private:
    juce::Justification justification = juce::Justification::centred;
};
