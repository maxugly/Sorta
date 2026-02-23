#include "UI/Components/TransportStrip.h"
#include "Core/AppEnums.h"
#include "Utils/Config.h"

TransportStrip::TransportStrip() {
    initialiseButtons();
}

void TransportStrip::initialiseButtons() {
    // Play/Stop Button
    addAndMakeVisible(playStopButton);
    playStopButton.setButtonText(Config::Labels::playButton);
    playStopButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);
    playStopButton.setEnabled(false);

    // Stop Button
    addAndMakeVisible(stopButton);
    stopButton.setButtonText(Config::Labels::stopButton);
    stopButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    stopButton.setEnabled(false);

    // Autoplay Button
    addAndMakeVisible(autoplayButton);
    autoplayButton.setButtonText(Config::Labels::autoplayButton);
    autoplayButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    autoplayButton.setClickingTogglesState(true);

    // Repeat Button
    addAndMakeVisible(repeatButton);
    repeatButton.setButtonText(Config::Labels::repeatButton);
    repeatButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);

    // Cut Button
    addAndMakeVisible(cutButton);
    cutButton.setButtonText(Config::Labels::cutButton);
    cutButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
    cutButton.setClickingTogglesState(true);
}

void TransportStrip::resized() {
    auto b = getLocalBounds();
    const int buttonWidth = Config::Layout::buttonWidth;
    const int spacing = (int)Config::UI::GroupSpacing;

    playStopButton.setBounds(b.removeFromLeft(buttonWidth));
    b.removeFromLeft(spacing);
    stopButton.setBounds(b.removeFromLeft(buttonWidth));
    b.removeFromLeft(spacing);
    autoplayButton.setBounds(b.removeFromLeft(buttonWidth));
    b.removeFromLeft(spacing);
    repeatButton.setBounds(b.removeFromLeft(buttonWidth));
    b.removeFromLeft(spacing);
    cutButton.setBounds(b.removeFromLeft(buttonWidth));
}

void TransportStrip::updatePlayButtonText(bool isPlaying) {
    playStopButton.setButtonText(isPlaying ? Config::Labels::stopButton
                                           : Config::Labels::playButton);
}

void TransportStrip::updateCutModeState(bool isCutModeActive) {
    cutButton.setToggleState(isCutModeActive, juce::dontSendNotification);
}

void TransportStrip::updateAutoplayState(bool isAutoplayActive) {
    autoplayButton.setToggleState(isAutoplayActive, juce::dontSendNotification);
}

void TransportStrip::updateRepeatState(bool isRepeating) {
    repeatButton.setToggleState(isRepeating, juce::dontSendNotification);
}
