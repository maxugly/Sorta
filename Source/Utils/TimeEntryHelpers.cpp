

#include "Utils/TimeEntryHelpers.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

namespace TimeEntryHelpers {
void handleTimeSegmentHighlight(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return;

    const bool isNegative = editor->getText().startsWith("-");
    const int offset = isNegative ? 1 : 0;
    const int charIndex = editor->getTextIndexAt(event.getPosition());
    if (charIndex < 0)
        return;

    const int effectiveIndex = charIndex - offset;
    juce::Range<int> newRange;

    if (effectiveIndex <= 1)
        newRange = juce::Range<int>(0 + offset, 2 + offset);
    else if (effectiveIndex >= 3 && effectiveIndex <= 4)
        newRange = juce::Range<int>(3 + offset, 5 + offset);
    else if (effectiveIndex >= 6 && effectiveIndex <= 7)
        newRange = juce::Range<int>(6 + offset, 8 + offset);
    else if (effectiveIndex >= 9 && effectiveIndex <= 13)
        newRange = juce::Range<int>(9 + offset, 14 + offset);
    else
        return;

    juce::MessageManager::callAsync([editor, newRange] {
        if (editor != nullptr)
            editor->setHighlightedRegion(newRange);
    });
}

double handleTimeStep(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel,
                      double currentVal, double sampleRate) {
    if (wheel.deltaY == 0.0f)
        return currentVal;

    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return currentVal;

    const bool isNegative = editor->getText().startsWith("-");
    const int offset = isNegative ? 1 : 0;
    const int charIndex = editor->getTextIndexAt(event.getPosition());
    const int effectiveIndex = charIndex - offset;

    const double step = calculateStepSize(effectiveIndex, event.mods, sampleRate);
    const double direction = (wheel.deltaY > 0) ? 1.0 : -1.0;

    return currentVal + (direction * step);
}

void validateTimeEntry(juce::TextEditor &editor, double totalLength, double sampleRate) {
    const double newPosition = TimeUtils::parseTime(editor.getText(), sampleRate);

    if (newPosition >= 0.0 && newPosition <= totalLength) {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::cutText);
    } else if (newPosition == -1.0) {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    } else {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorWarning);
    }
}

double calculateStepSize(int charIndex, const juce::ModifierKeys &mods, double sampleRate) {
    double step = (sampleRate > 0.0) ? (1.0 / sampleRate) : 0.0001;
    bool isSamples = false;

    if (charIndex >= 0 && charIndex <= 1)
        step = Config::Audio::cutStepHours;
    else if (charIndex >= 3 && charIndex <= 4)
        step = Config::Audio::cutStepMinutes;
    else if (charIndex >= 6 && charIndex <= 7)
        step = Config::Audio::cutStepSeconds;
    else if (charIndex >= 9)
        isSamples = true;

    if (isSamples) {
        if (mods.isCtrlDown() && mods.isShiftDown()) {
            step *= 0.1; // Sub-sample precision if supported? 
        } else if (mods.isShiftDown()) {
            step *= 1.0; // Already 1 sample
        }
    } else {
        double multiplier = 1.0;
        if (mods.isShiftDown() && mods.isCtrlDown())
            multiplier = 0.01;
        else if (mods.isShiftDown())
            multiplier = 0.1;

        step *= multiplier;
    }

    if (mods.isAltDown())
        step *= 10.0;

    return step;
}

float getZoomFactorForPosition(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return 10.0f;

    const bool isNegative = editor->getText().startsWith("-");
    const int offset = isNegative ? 1 : 0;
    const int charIndex = editor->getTextIndexAt(event.getPosition());
    if (charIndex < 0)
        return 10.0f;

    const int effectiveIndex = charIndex - offset;

    // Based on TimeUtils::formatTime result: "HH:MM:SS:sssss"
    if (effectiveIndex <= 1)
        return 1.0f;
    if (effectiveIndex >= 3 && effectiveIndex <= 4)
        return 10.0f;
    if (effectiveIndex >= 6 && effectiveIndex <= 7)
        return 100.0f;
    if (effectiveIndex >= 9)
        return 10000.0f;

    return 10.0f;
}
} // namespace TimeEntryHelpers
