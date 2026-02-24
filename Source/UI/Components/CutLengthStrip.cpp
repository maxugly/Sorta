#include "UI/Components/CutLengthStrip.h"
#include "Utils/Config.h"

CutLengthStrip::CutLengthStrip() {
    addAndMakeVisible(lengthEditor);
    lengthEditor.setReadOnly(false);
    lengthEditor.setJustification(juce::Justification::centred);
    lengthEditor.setColour(juce::TextEditor::backgroundColourId, Config::Colors::transparentBlack);
    lengthEditor.setColour(juce::TextEditor::outlineColourId, Config::Colors::transparentBlack);
    lengthEditor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    lengthEditor.setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    lengthEditor.setMultiLine(false);
    lengthEditor.setReturnKeyStartsNewLine(false);
    lengthEditor.setSelectAllWhenFocused(true);
}

void CutLengthStrip::resized() {
    lengthEditor.setBounds(getLocalBounds());
}
