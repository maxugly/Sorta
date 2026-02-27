#include "StyledTextEditor.h"
#include "Utils/Config.h"

StyledTextEditor::StyledTextEditor() : juce::TextEditor(), justification(juce::Justification::centred) {}

void StyledTextEditor::setCustomTextColor(juce::Colour newColor) {
    customTextColor = newColor;
    lastText.clear(); // Force layout rebuild
    repaint();
}

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
        if (text != lastText || customTextColor != lastColor || getWidth() != lastWidth) {
            juce::AttributedString as;
            as.setJustification(getJustification());
            auto symbolColor = customTextColor.overlaidWith(juce::Colours::white.withAlpha(0.85f));
            
            for (int i = 0; i < text.length(); ++i) {
                auto c = text[i];
                bool isSymbol = (c == ':' || c == '+' || c == '-');
                as.append(juce::String::charToString(c), getFont(), isSymbol ? symbolColor : customTextColor);
            }
            textLayout.createLayout(as, (float)getWidth());
            lastText = text;
            lastColor = customTextColor;
            lastWidth = getWidth();
        }

        auto textHeight = textLayout.getHeight();
        float yOffset = ((float)getHeight() - textHeight) / 2.0f;
        textLayout.draw(g, juce::Rectangle<float>(0.0f, yOffset, (float)getWidth(), textHeight));
    }

    // 3. Draw Outline
    getLookAndFeel().drawTextEditorOutline(g, getWidth(), getHeight(), *this);
}

void StyledTextEditor::focusGained(FocusChangeType cause) {
    juce::TextEditor::focusGained(cause);
    setColour(juce::TextEditor::textColourId, customTextColor);
    repaint();
}

void StyledTextEditor::focusLost(FocusChangeType cause) {
    juce::TextEditor::focusLost(cause);
    // Make text transparent when not focused to avoid double-drawing with our custom paint()
    setColour(juce::TextEditor::textColourId, juce::Colours::transparentBlack);
    repaint();
}
