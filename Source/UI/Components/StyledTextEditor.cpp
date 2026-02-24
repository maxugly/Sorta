#include "StyledTextEditor.h"
#include "Utils/Config.h"

StyledTextEditor::StyledTextEditor() : juce::TextEditor() {}

void StyledTextEditor::applyStandardStyle(juce::Justification justification) {
    setReadOnly(false);
    setJustification(justification);
    setColour(juce::TextEditor::backgroundColourId, Config::Colors::Button::base);
    setColour(juce::TextEditor::outlineColourId, Config::Colors::Button::outline);
    setColour(juce::TextEditor::textColourId, Config::Colors::Button::text);
    setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    setMultiLine(false);
    setReturnKeyStartsNewLine(false);
    setSelectAllWhenFocused(true);
}
