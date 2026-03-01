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

void PlaybackTimeView::setSplitLayout(bool shouldSplit) {
    if (isSplit != shouldSplit) {
        isSplit = shouldSplit;
        resized();
    }
}

void PlaybackTimeView::paint(juce::Graphics &g) {
    juce::ignoreUnused(g);
}

void PlaybackTimeView::resized() {
    const int playbackWidth = Config::Layout::Text::playbackWidth;
    const int playbackHeight = Config::Layout::Text::playbackHeight;
    const int margin = Config::Layout::windowBorderMargins;
    auto bounds = getLocalBounds();

    if (isSplit) {
        // Hide total time entirely during cut mode to clear the center!
        totalTimeEditor.setVisible(false);
        elapsedTimeEditor.setBounds(bounds.removeFromLeft(playbackWidth).withSizeKeepingCentre(playbackWidth, playbackHeight).translated(margin, 0));
        remainingTimeEditor.setBounds(bounds.removeFromRight(playbackWidth).withSizeKeepingCentre(playbackWidth, playbackHeight).translated(-margin, 0));
    } else {
        totalTimeEditor.setVisible(true);
        const int totalWidth = playbackWidth * 3;
        auto groupBounds = bounds.withSizeKeepingCentre(totalWidth, playbackHeight);

        elapsedTimeEditor.setBounds(groupBounds.removeFromLeft(playbackWidth));
        totalTimeEditor.setBounds(groupBounds.removeFromLeft(playbackWidth));
        remainingTimeEditor.setBounds(groupBounds);
    }
}
