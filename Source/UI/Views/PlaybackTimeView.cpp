/**
 * @file PlaybackTimeView.cpp
 */

#include "UI/Views/PlaybackTimeView.h"
#include "Utils/Config.h"
#include "UI/ControlPanel.h"

PlaybackTimeView::PlaybackTimeView() {
    addAndMakeVisible(elapsedTimeEditor);
    elapsedTimeEditor.applyStandardStyle(juce::Justification::left);

    addAndMakeVisible(remainingTimeEditor);
    remainingTimeEditor.applyStandardStyle(juce::Justification::right);
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
