#ifndef AUDIOFILER_TIMEENTRYHELPERS_H
#define AUDIOFILER_TIMEENTRYHELPERS_H

#include <JuceHeader.h>

/**
 * @file TimeEntryHelpers.h
 * @ingroup Helpers
 * @namespace TimeEntryHelpers
 * @brief Helper functions for validating and manipulating time strings in text editors.
 */
namespace TimeEntryHelpers {

/**
 * @brief Highlights the time segment (HH, MM, SS, ms) under the mouse.
 * @param event The mouse event containing position and component.
 */
void handleTimeSegmentHighlight(const juce::MouseEvent &event);

/**
 * @brief Calculates the new time value based on mouse wheel movement over a segment.
 * @param event The mouse event.
 * @param wheel The wheel details.
 * @param currentVal The current time in seconds.
 * @param sampleRate The sample rate for fine stepping.
 * @return The new time in seconds.
 */
double handleTimeStep(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel,
                      double currentVal, double sampleRate);

/**
 * @brief Validates the time entered in a text editor and updates its color.
 * @details Checks if the text is a valid timestamp (MM:SS.ms) and if it falls
 *          within the valid range (0 to totalLength). Updates the text color
 *          using `Config::Colors` to indicate errors or warnings.
 *
 * @param editor The TextEditor to validate.
 * @param totalLength The maximum allowed time in seconds.
 */
void validateTimeEntry(juce::TextEditor &editor, double totalLength);

/**
 * @brief Calculates the time step size based on cursor position and modifier keys.
 * @details Used for "scrubbing" time values with the mouse wheel or arrow keys.
 *          The step size changes depending on which part of the time string (minutes,
 *          seconds, milliseconds) the cursor is hovering over.
 *
 * @param charIndex The character index of the cursor or mouse hover.
 * @param mods The modifier keys currently pressed.
 * @param sampleRate The audio sample rate (used for fine adjustments).
 * @return The step size in seconds.
 */
double calculateStepSize(int charIndex, const juce::ModifierKeys &mods, double sampleRate = 0.0);
} // namespace TimeEntryHelpers

#endif
