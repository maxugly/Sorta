#include "StyledTextEditor.h"
#include "Utils/Config.h"

StyledTextEditor::StyledTextEditor() : juce::TextEditor(), justification(juce::Justification::centred) {}

void StyledTextEditor::setCustomTextColor(juce::Colour color) {
    setColour(juce::TextEditor::textColourId, color);
    applyColourToAllText(color);
}

juce::Colour StyledTextEditor::getCustomTextColor() const {
    return findColour(juce::TextEditor::textColourId);
}

void StyledTextEditor::applyStandardStyle(juce::Justification j) {
    setReadOnly(false);
    setJustification(j);
    setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    setMultiLine(false);
    setReturnKeyStartsNewLine(false);
    setSelectAllWhenFocused(true);

    setBorder(juce::BorderSize<int>(0));
    setIndents(0, 0);
}

