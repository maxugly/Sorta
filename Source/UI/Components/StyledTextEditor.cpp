#include "StyledTextEditor.h"
#include "Utils/Config.h"

StyledTextEditor::StyledTextEditor() : juce::TextEditor() {}

void StyledTextEditor::applyStandardStyle(juce::Justification justification) {
    setReadOnly(false);
    setJustification(justification);
    setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    setMultiLine(false);
    setReturnKeyStartsNewLine(false);
    setSelectAllWhenFocused(true);
}
