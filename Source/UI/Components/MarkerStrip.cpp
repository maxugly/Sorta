#include "UI/Components/MarkerStrip.h"
#include "Core/AppEnums.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

MarkerStrip::MarkerStrip(MarkerType type)
    : markerType(type) {
    initialiseComponents();
}

void MarkerStrip::initialiseComponents() {
    // Marker Button (In/Out)
    addAndMakeVisible(markerButton);
    markerButton.setButtonText(markerType == MarkerType::In ? Config::Labels::cutInButton
                                                           : Config::Labels::cutOutButton);
    markerButton.getProperties().set("GroupPosition", markerType == MarkerType::In
                                                           ? (int)AppEnums::GroupPosition::Left
                                                           : (int)AppEnums::GroupPosition::Right);

    // Timer Editor
    addAndMakeVisible(timerEditor);
    timerEditor.setReadOnly(false);
    timerEditor.setJustification(juce::Justification::centred);
    timerEditor.setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    timerEditor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    timerEditor.setFont(juce::Font(juce::FontOptions(Config::Layout::Text::playbackSize)));
    timerEditor.setInputRestrictions(12, "0123456789:.");
    timerEditor.setMultiLine(false);
    timerEditor.setReturnKeyStartsNewLine(false);
    timerEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);

    // Reset Button
    addAndMakeVisible(resetButton);
    resetButton.setButtonText(Config::Labels::clearButton);
    resetButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);

    // Threshold Editor (percentage input)
    addAndMakeVisible(thresholdEditor);
    thresholdEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);

    // AutoCut Button
    addAndMakeVisible(autoCutButton);
    autoCutButton.setButtonText(markerType == MarkerType::In ? Config::Labels::autoCutInButton
                                                             : Config::Labels::autoCutOutButton);
    autoCutButton.setClickingTogglesState(true);
    autoCutButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);

    // Lock Button
    addAndMakeVisible(lockButton);
    lockButton.setButtonText(Config::Labels::lockUnlocked);
    lockButton.setClickingTogglesState(true);
    lockButton.getProperties().set("GroupPosition", markerType == MarkerType::In
                                                       ? (int)AppEnums::GroupPosition::Right
                                                       : (int)AppEnums::GroupPosition::Left);
}

void MarkerStrip::resized() {
    auto b = getLocalBounds();
    const float unit = Config::UI::WidgetUnit;
    const int spacing = (int)Config::UI::GroupSpacing;

    const int markerWidth = (int)(Config::UI::CutButtonWidthUnits * unit);
    const int timerWidth = (int)(Config::UI::TimerWidthUnits * unit);
    const int resetWidth = (int)(Config::UI::ResetButtonWidthUnits * unit);
    const int thresholdWidth = (int)(Config::UI::ThresholdWidthUnits * unit);
    const int autoCutWidth = (int)(Config::UI::CutButtonWidthUnits * unit);
    const int lockWidth = (int)(Config::UI::ResetButtonWidthUnits * unit);

    if (markerType == MarkerType::In) {
        // [In(L), Timer, Reset, Threshold, AutoCut, Lock(R)]
        markerButton.setBounds(b.removeFromLeft(markerWidth));
        b.removeFromLeft(spacing);
        timerEditor.setBounds(b.removeFromLeft(timerWidth));
        b.removeFromLeft(spacing);
        resetButton.setBounds(b.removeFromLeft(resetWidth));
        b.removeFromLeft(spacing);
        thresholdEditor.setBounds(b.removeFromLeft(thresholdWidth));
        b.removeFromLeft(spacing);
        autoCutButton.setBounds(b.removeFromLeft(autoCutWidth));
        b.removeFromLeft(spacing);
        lockButton.setBounds(b.removeFromLeft(lockWidth));
    } else {
        // [Lock(L), AutoCut, Threshold, Reset, Timer, Out(R)]
        lockButton.setBounds(b.removeFromLeft(lockWidth));
        b.removeFromLeft(spacing);
        autoCutButton.setBounds(b.removeFromLeft(autoCutWidth));
        b.removeFromLeft(spacing);
        thresholdEditor.setBounds(b.removeFromLeft(thresholdWidth));
        b.removeFromLeft(spacing);
        resetButton.setBounds(b.removeFromLeft(resetWidth));
        b.removeFromLeft(spacing);
        timerEditor.setBounds(b.removeFromLeft(timerWidth));
        b.removeFromLeft(spacing);
        markerButton.setBounds(b.removeFromLeft(markerWidth));
    }
}

void MarkerStrip::updateTimerText(const juce::String &text) {
    if (timerEditor.getText() != text)
        timerEditor.setText(text, juce::dontSendNotification);
}

void MarkerStrip::updateAutoCutState(bool isActive) {
    autoCutButton.setToggleState(isActive, juce::dontSendNotification);
}

void MarkerStrip::updateMarkerButtonColor(juce::Colour color) {
    markerButton.setColour(juce::TextButton::buttonColourId, color);
}
