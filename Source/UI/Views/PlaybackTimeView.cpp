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
        ed.setColour(juce::TextEditor::backgroundColourId, Config::Colors::transparentBlack);
        ed.setColour(juce::TextEditor::outlineColourId, Config::Colors::transparentBlack);
        ed.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        ed.setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::playbackSize)));
        ed.applyFontToAllText(ed.getFont());
        ed.setMultiLine(false);
        ed.setReturnKeyStartsNewLine(false);
        ed.setSelectAllWhenFocused(true);
    };

    configure(elapsedTimeEditor, juce::Justification::left);
    configure(remainingTimeEditor, juce::Justification::right);
    configure(cutLengthEditor, juce::Justification::centred);
}

PlaybackTimeView::~PlaybackTimeView() = default;

void PlaybackTimeView::paint(juce::Graphics &g) {
    if (totalTimeString.isEmpty()) return;

    g.setColour(Config::Colors::playbackText);
    g.setFont((float)Config::Layout::Text::playbackSize);

    juce::String displayStr = " / " + totalTimeString;
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    const int textX = (getWidth() / 2);

    g.drawText(displayStr, textX + (playbackWidth / 2), 0,
               playbackWidth / 2, getHeight(),
               juce::Justification::left, false);
}

void PlaybackTimeView::resized() {
    const int margin = Config::Layout::windowBorderMargins;
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    const int playbackHeight = Config::Layout::Text::playbackHeight;
    auto bounds = getLocalBounds();

    elapsedTimeEditor.setBounds(margin, 0, playbackWidth, playbackHeight);
    remainingTimeEditor.setBounds(bounds.getRight() - margin - playbackWidth, 0, playbackWidth, playbackHeight);
    cutLengthEditor.setBounds((bounds.getWidth() / 2) - (playbackWidth / 2), 0, playbackWidth / 2, playbackHeight);
}
