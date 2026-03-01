#include "Utils/TimeEntryHelpers.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

namespace TimeEntryHelpers {

/**
 * @details This method implements a "Geometric Proportional Hit-Zone" algorithm to 
 *          map raw mouse coordinates to specific characters in a TextEditor.
 *          
 *          Why this is necessary:
 *          Standard JUCE `TextEditor` components often have internal "invisible padding" 
 *          or non-linear character spacing that makes standard `getTextIndexAt()` 
 *          calls unreliable for frame-accurate time manipulation.
 *          
 *          Algorithmic Steps:
 *          1. **Static Baseline Calculation**: We calculate the `textWidth` using 
 *             the editor's font metrics to find the absolute center-aligned start X.
 *          2. **Linear Accumulation**: We iterate through each character, 
 *             accumulating its specific width (`getStringWidth`).
 *          3. **Hit-Box Intersection**: As soon as the `mouseX` coordinate falls 
 *             within a character's accumulated width window, we return that index. 
 *             This bypasses the native JUCE padding bug by relying on pure 
 *             mathematical string measurements rather than the component's 
 *             internal rendering state.
 */
static int calculateCharIndexAtX(const juce::TextEditor& editor, int mouseX) {
    const auto& font = editor.getFont();
    const auto text = editor.getText();
    const int textWidth = font.getStringWidth(text);
    // Determine the starting X offset for center-aligned text
    const int startX = (editor.getWidth() - textWidth) / 2;
    int currentX = startX;

    for (int i = 0; i < text.length(); ++i) {
        int charWidth = font.getStringWidth(juce::String::charToString(text[i]));
        // If the mouse is within the horizontal bounds of this specific character
        if (mouseX < currentX + charWidth) return i;
        currentX += charWidth;
    }
    return text.length() - 1;
}

void handleTimeSegmentHighlight(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr) return;

    const auto text = editor->getText();
    const bool hasPrefix = text.startsWith("-") || text.startsWith("+");
    const int offset = hasPrefix ? 1 : 0;
    const int charIndex = calculateCharIndexAtX(*editor, event.x);
    const int effectiveIndex = juce::jlimit(0, (int)editor->getText().length() - 1, charIndex) - offset;
    juce::Range<int> newRange;

    if (effectiveIndex <= 2)      newRange = juce::Range<int>(0 + offset, 2 + offset);
    else if (effectiveIndex <= 5) newRange = juce::Range<int>(3 + offset, 5 + offset);
    else if (effectiveIndex <= 8) newRange = juce::Range<int>(6 + offset, 8 + offset);
    else                          newRange = juce::Range<int>(9 + offset, 14 + offset);

    juce::MessageManager::callAsync([editor, newRange] {
        if (editor != nullptr)
            editor->setHighlightedRegion(newRange);
    });
}

double handleTimeStep(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel,
                      double currentVal, double sampleRate) {
    if (wheel.deltaY == 0.0f) return currentVal;
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr) return currentVal;

    const auto text = editor->getText();
    const bool hasPrefix = text.startsWith("-") || text.startsWith("+");
    const int offset = hasPrefix ? 1 : 0;
    const int charIndex = calculateCharIndexAtX(*editor, event.x);
    const int effectiveIndex = juce::jlimit(0, (int)editor->getText().length() - 1, charIndex) - offset;

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

/**
 * @details This method calculates the discrete time adjustment "step" based on which 
 *          segment of the HH:MM:SS.mmm timecode the mouse is currently hovering over.
 *          
 *          Mathematical Model:
 *          1. **Segmented Multipliers**:
 *             - Index 0-2 (HH): Sets the step to 3600 seconds.
 *             - Index 3-5 (MM): Sets the step to 60 seconds.
 *             - Index 6-8 (SS): Sets the step to 1 second.
 *             - Index 9+ (mmm): Sets the step to 1/sampleRate (single-sample precision).
 *          2. **Modifier Sensitivity**:
 *             - `Shift`: Reduces the step by 10x for fine-tuning.
 *             - `Shift + Ctrl`: Reduces the step by 100x for surgical precision.
 *             - `Alt`: Increases the step by 10x for rapid "scrubbing" through the timecode.
 *          3. **Sample-Rate Normalization**: If the hover is in the millisecond/sample 
 *             segment, the step is normalized against the `sampleRate` to ensure 
 *             sub-millisecond movements are frame-accurate.
 */
double calculateStepSize(int charIndex, const juce::ModifierKeys &mods, double sampleRate) {
    // Default to single-sample precision
    double step = (sampleRate > 0.0) ? (1.0 / sampleRate) : 0.0001;
    bool isSamples = false;

    // Route the step size based on the character index hit-zone
    if (charIndex <= 2)      step = Config::Audio::cutStepHours;
    else if (charIndex <= 5) step = Config::Audio::cutStepMinutes;
    else if (charIndex <= 8) step = Config::Audio::cutStepSeconds;
    else                     isSamples = true;

    if (isSamples) {
        // Special case for sub-sample precision modifiers
        if (mods.isCtrlDown() && mods.isShiftDown()) {
            step *= 0.1; 
        } else if (mods.isShiftDown()) {
            step *= 1.0; // Fixed 1 sample
        }
    } else {
        // Apply geometric reduction based on modifier keys for time-based segments
        double multiplier = 1.0;
        if (mods.isShiftDown() && mods.isCtrlDown())
            multiplier = 0.01;
        else if (mods.isShiftDown())
            multiplier = 0.1;

        step *= multiplier;
    }

    // Apply the "Fast Scrub" multiplier
    if (mods.isAltDown())
        step *= 10.0;

    return step;
}

float getZoomFactorForPosition(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr) return 10.0f;

    const auto text = editor->getText();
    const bool hasPrefix = text.startsWith("-") || text.startsWith("+");
    const int offset = hasPrefix ? 1 : 0;
    const int charIndex = calculateCharIndexAtX(*editor, event.x);
    const int effectiveIndex = juce::jlimit(0, (int)editor->getText().length() - 1, charIndex) - offset;

    if (effectiveIndex <= 2) return 1.0f;
    if (effectiveIndex <= 5) return 10.0f;
    if (effectiveIndex <= 8) return 100.0f;
    return 10000.0f;
}

bool shouldShowZoomPopup(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr) return false;
    const auto text = editor->getText();
    const bool hasPrefix = text.startsWith("-") || text.startsWith("+");
    const int offset = hasPrefix ? 1 : 0;
    const int charIndex = calculateCharIndexAtX(*editor, event.x);
    const int effectiveIndex = juce::jlimit(0, (int)editor->getText().length() - 1, charIndex) - offset;
    return effectiveIndex >= 6;
}
} // namespace TimeEntryHelpers
