#include "UI/Components/CutLengthStrip.h"
#include "Core/AppEnums.h"
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

    addAndMakeVisible(lockButton);
    lockButton.setButtonText(Config::Labels::lockUnlocked);
    lockButton.setClickingTogglesState(true);
    lockButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
}

void CutLengthStrip::resized() {
    auto b = getLocalBounds();
    const int lockWidth = (int)(Config::UI::ResetButtonWidthUnits * Config::UI::WidgetUnit);
    lockButton.setBounds(b.removeFromRight(lockWidth));
    lengthEditor.setBounds(b);
}

void CutLengthStrip::paint(juce::Graphics& g) {
    auto bounds = lengthEditor.getBounds().toFloat().reduced(Config::UI::ButtonOutlineThickness / 2.0f);
    auto cornerSize = Config::UI::ButtonCornerSize;
    
    juce::Path p;
    // Corners: topLeft=true, topRight=false, bottomLeft=true, bottomRight=false (GroupPosition::Left style)
    p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          cornerSize, cornerSize, true, false, true, false);
                          
    g.setColour(Config::Colors::Button::base);
    g.fillPath(p);
    
    g.setColour(Config::Colors::Button::outline);
    g.strokePath(p, juce::PathStrokeType(Config::UI::ButtonOutlineThickness));
}
