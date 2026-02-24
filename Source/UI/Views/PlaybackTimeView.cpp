/**
 * @file PlaybackTimeView.cpp
 */

#include "UI/Views/PlaybackTimeView.h"
#include "Utils/Config.h"
#include "UI/ControlPanel.h"

PlaybackTimeView::PlaybackTimeView() {
    auto configure = [&](juce::TextEditor &ed, juce::Justification just) {
        addAndMakeVisible(ed);
        ed.setReadOnly(false);
        ed.setJustification(just);
        ed.setColour(juce::TextEditor::backgroundColourId, Config::Colors::Window::background);
        ed.setColour(juce::TextEditor::outlineColourId, Config::Colors::Window::background);
        ed.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        ed.setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
        ed.applyFontToAllText(ed.getFont());
        ed.setMultiLine(false);
        ed.setReturnKeyStartsNewLine(false);
        ed.setSelectAllWhenFocused(true);
    };

    configure(elapsedTimeEditor, juce::Justification::left);
    configure(remainingTimeEditor, juce::Justification::right);
}

PlaybackTimeView::~PlaybackTimeView() = default;

void PlaybackTimeView::paint(juce::Graphics &g) {
    if (totalTimeString.isEmpty()) return;

    g.setColour(Config::Colors::playbackText);
    g.setFont((float)Config::Layout::Text::playbackSize);

    juce::String displayStr = Config::Labels::timeSeparator + totalTimeString;
    const int margin = Config::Layout::windowBorderMargins;
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    
    // Total time belongs immediately after the Elapsed Time Editor (left side)
    const int textX = margin + playbackWidth;

    g.drawText(displayStr, textX, 0,
               playbackWidth, getHeight(),
               juce::Justification::left, false);
}

void PlaybackTimeView::resized() {
    const int margin = Config::Layout::windowBorderMargins;
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    const int playbackHeight = Config::Layout::Text::playbackHeight;
    auto bounds = getLocalBounds();

    elapsedTimeEditor.setBounds(margin, 0, playbackWidth, playbackHeight);
    remainingTimeEditor.setBounds(bounds.getRight() - margin - playbackWidth, 0, playbackWidth, playbackHeight);
}
