

#include "Presenters/StatsPresenter.h"

#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"
#include "Utils/TimeUtils.h"
#include <cmath>

StatsPresenter::StatsPresenter(ControlPanel &ownerIn) : owner(ownerIn) {
    owner.addAndMakeVisible(statsOverlay);
    auto &statsDisplay = statsOverlay.statsDisplay;
    statsDisplay.setReadOnly(true);
    statsDisplay.setMultiLine(true);
    statsDisplay.setWantsKeyboardFocus(false);
    statsDisplay.setColour(juce::TextEditor::backgroundColourId, Config::Colors::Window::background);
    statsDisplay.setColour(juce::TextEditor::outlineColourId, Config::Colors::Window::background);
    statsDisplay.setColour(juce::TextEditor::textColourId, Config::Colors::statsText);
    statsOverlay.setVisible(false);

    statsOverlay.onHeightChanged = [this](int newHeight) { currentHeight = newHeight; };
    owner.getSessionState().addListener(this);
}

StatsPresenter::~StatsPresenter() {
    owner.getSessionState().removeListener(this);
}

void StatsPresenter::fileChanged(const juce::String &filePath) {
    juce::ignoreUnused(filePath);
    updateStats();
}

void StatsPresenter::updateStats() {
    setDisplayText(buildStatsString(), Config::Colors::statsText);
}

void StatsPresenter::toggleVisibility() {
    setShouldShowStats(!showStats);
}

void StatsPresenter::setShouldShowStats(bool shouldShowStats) {
    showStats = shouldShowStats;

    updateVisibility();
    owner.resized();
}

juce::String StatsPresenter::getStatsText() const {
    return statsOverlay.statsDisplay.getText();
}

void StatsPresenter::layoutWithin(const juce::Rectangle<int> &contentAreaBounds) {
    auto statsBounds = contentAreaBounds.withHeight(currentHeight)
                           .reduced(Config::Layout::Stats::sideMargin, 0)
                           .translated(0, Config::Layout::Stats::topMargin);
    if (showStats) {
        statsOverlay.setBounds(statsBounds);
        statsOverlay.toFront(true);
    }

    updateVisibility();
}

void StatsPresenter::setDisplayText(const juce::String &text, juce::Colour color) {
    statsOverlay.statsDisplay.setText(text, juce::dontSendNotification);
    statsOverlay.statsDisplay.setColour(juce::TextEditor::textColourId, color);
}

juce::TextEditor &StatsPresenter::getDisplay() {
    return statsOverlay.statsDisplay;
}

void StatsPresenter::setDisplayEnabled(bool shouldEnable) {
    statsOverlay.statsDisplay.setEnabled(shouldEnable);
}

juce::String StatsPresenter::buildStatsString() const {
    juce::String stats;
    AudioPlayer &audioPlayer = owner.getAudioPlayer();
    auto &thumbnail = audioPlayer.getThumbnail();
    double sampleRate = 0.0;
    juce::int64 lengthInSamples = 0;

    if (thumbnail.getTotalLength() > 0.0 &&
        audioPlayer.getReaderInfo(sampleRate, lengthInSamples)) {
        stats << Config::Labels::statsFile << audioPlayer.getLoadedFile().getFileName() << "\n";
        stats << Config::Labels::statsSamples << lengthInSamples << "\n";
        stats << Config::Labels::statsRate << sampleRate << Config::Labels::statsHz << "\n";
        stats << Config::Labels::statsChannels << thumbnail.getNumChannels() << "\n";
        stats << Config::Labels::statsLength << TimeUtils::formatTime(thumbnail.getTotalLength()) << "\n";

        float minVal = 0.0f;
        float maxVal = 0.0f;
        thumbnail.getApproximateMinMax(0.0, thumbnail.getTotalLength(), 0, minVal, maxVal);
        stats << Config::Labels::statsPeak0 << juce::jmax(std::abs(minVal), std::abs(maxVal)) << "\n";
        stats << Config::Labels::statsMin << minVal << Config::Labels::statsMax << maxVal << "\n";

        if (thumbnail.getNumChannels() > 1) {
            thumbnail.getApproximateMinMax(0.0, thumbnail.getTotalLength(), 1, minVal, maxVal);
            stats << Config::Labels::statsPeak1 << juce::jmax(std::abs(minVal), std::abs(maxVal))
                  << "\n";
            stats << Config::Labels::statsMin << minVal << Config::Labels::statsMax << maxVal << "\n";
        }
    } else {
        stats << Config::Labels::statsError;
    }

    return stats;
}

void StatsPresenter::updateVisibility() {
    statsOverlay.setVisible(showStats);
    if (showStats)
        statsOverlay.toFront(true);
}
