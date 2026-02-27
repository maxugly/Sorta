#include "StyledTextEditor.h"
#include "Utils/Config.h"

StyledTextEditor::StyledTextEditor() : juce::TextEditor(), justification(juce::Justification::centred) {}

void StyledTextEditor::applyStandardStyle(juce::Justification j) {
    setReadOnly(false);
    setJustification(j);
    setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    setMultiLine(false);
    setReturnKeyStartsNewLine(false);
    setSelectAllWhenFocused(true);
    setColour(juce::TextEditor::textColourId, juce::Colours::transparentBlack);
}

void StyledTextEditor::paint(juce::Graphics& g) {
    if (hasKeyboardFocus(true)) {
        juce::TextEditor::paint(g);
        return;
    }

    // 1. Draw Background
    getLookAndFeel().fillTextEditorBackground(g, getWidth(), getHeight(), *this);

    // 2. Draw Text
    auto text = getText();
    if (text.isNotEmpty()) {
        juce::Colour textColor = juce::Colours::white;
        if (getProperties().contains("CustomTextColor")) {
            textColor = juce::Colour::fromString(getProperties()["CustomTextColor"].toString());
        } else {
            textColor = findColour(juce::TextEditor::textColourId);
        }

        // Make symbols very light version of the text color
        auto symbolColor = textColor.overlaidWith(juce::Colours::white.withAlpha(0.85f));

        juce::AttributedString as;
        as.setJustification(getJustification());
        
        for (int i = 0; i < text.length(); ++i) {
            auto c = text[i];
            bool isSymbol = (c == ':' || c == '+' || c == '-');
            as.append(juce::String::charToString(c), getFont(), isSymbol ? symbolColor : textColor);
        }

        juce::TextLayout layout;
        layout.createLayout(as, (float)getWidth());
        
        // Center vertically in the component
        auto textHeight = layout.getHeight();
        float yOffset = ((float)getHeight() - textHeight) / 2.0f;
        
        layout.draw(g, juce::Rectangle<float>(0.0f, yOffset, (float)getWidth(), textHeight));
    }

    // 3. Draw Outline
    getLookAndFeel().drawTextEditorOutline(g, getWidth(), getHeight(), *this);
}

void StyledTextEditor::focusGained(FocusChangeType cause) {
    juce::TextEditor::focusGained(cause);
    
    // Restore text colour when focused so default JUCE editing works
    if (getProperties().contains("CustomTextColor")) {
        auto textColor = juce::Colour::fromString(getProperties()["CustomTextColor"].toString());
        setColour(juce::TextEditor::textColourId, textColor);
    }
    repaint();
}

void StyledTextEditor::focusLost(FocusChangeType cause) {
    juce::TextEditor::focusLost(cause);
    
    // Make text transparent when not focused to avoid double-drawing with our custom paint()
    setColour(juce::TextEditor::textColourId, juce::Colours::transparentBlack);
    repaint();
}
