/**
 * @file PlaybackTimeView.cpp
 */

#include "UI/Views/PlaybackTimeView.h"
#include "Utils/Config.h"
#include "UI/ControlPanel.h"

PlaybackTimeView::PlaybackTimeView() {
    addAndMakeVisible(elapsedTimeEditor);
    elapsedTimeEditor.applyStandardStyle(juce::Justification::centred);
    elapsedTimeEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);

    addAndMakeVisible(totalTimeEditor);
    totalTimeEditor.applyStandardStyle(juce::Justification::centred);
    totalTimeEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    totalTimeEditor.setReadOnly(true); // Track length is usually read-only

    addAndMakeVisible(remainingTimeEditor);
    remainingTimeEditor.applyStandardStyle(juce::Justification::centred);
    remainingTimeEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
}

PlaybackTimeView::~PlaybackTimeView() = default;

void PlaybackTimeView::paint(juce::Graphics &g) {
    juce::ignoreUnused(g);
}

void PlaybackTimeView::resized() {
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    const int playbackHeight = Config::Layout::Text::playbackHeight;
    auto bounds = getLocalBounds();

    const int totalWidth = playbackWidth * 3;
    auto groupBounds = bounds.withSizeKeepingCentre(totalWidth, playbackHeight);

    elapsedTimeEditor.setBounds(groupBounds.removeFromLeft(playbackWidth));
    totalTimeEditor.setBounds(groupBounds.removeFromLeft(playbackWidth));
    remainingTimeEditor.setBounds(groupBounds);
}
