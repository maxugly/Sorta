#ifndef AUDIOFILER_TIMEENTRYHELPERS_H
#define AUDIOFILER_TIMEENTRYHELPERS_H

#include <JuceHeader.h>

/**
 * @file TimeEntryHelpers.h
 * @ingroup Helpers
 * @namespace TimeEntryHelpers
 * @brief Logic-tier utilities for validating and manipulating time strings in text editors.
 * 
 * @details Architecturally, TimeEntryHelpers serves as an "Interaction Provider" 
 *          that bridges the gap between raw user input (text strings) and 
 *          temporal audio coordinates (seconds). It centralizes the parsing, 
 *          formatting, and scrubbing logic used by `StyledTextEditor` and 
 *          time-related Presenters.
 * 
 *          Key capabilities:
 *          - **Context-Aware Scrubbing**: Calculates precise time steps based on 
 *            which segment of a timestamp (HH:MM:SS.ms) is being hovered or manipulated.
 *          - **Visual Validation**: Real-time coloring of text editors based on 
 *            out-of-range or malformed inputs, providing instant user feedback.
 *          - **Zoom Interaction**: Provides the mathematical basis for showing 
 *            localized waveform "zoom popups" when hovering over specific time fields.
 * 
 * @see StyledTextEditor
 * @see TimeUtils
 * @see Config::Colors
 */
namespace TimeEntryHelpers {

/**
 * @brief Highlights the specific time segment (HH, MM, SS, or ms) currently under the mouse.
 * @details This method analyzes the local cursor position relative to the text layout 
 *          and applies a selection or background highlight to the relevant 
 *          characters to indicate "focus" for scrubbing.
 * @param event The mouse event containing position and parent component context.
 */
void handleTimeSegmentHighlight(const juce::MouseEvent &event);

/**
 * @brief Calculates a new time value based on mouse wheel movement over a timestamp segment.
 * @details The calculation is context-sensitive: scrolling over the "seconds" field 
 *          increments by 1.0s, while scrolling over "milliseconds" increments by 0.001s.
 * @param event The mouse event capturing the hover position.
 * @param wheel Details of the scroll amount and direction.
 * @param currentVal The baseline time in seconds before the adjustment.
 * @param sampleRate The audio sample rate, used to ensure sub-millisecond precision 
 *                   during fine-stepping.
 * @return The new calculated time in seconds.
 */
double handleTimeStep(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel,
                      double currentVal, double sampleRate);

/**
 * @brief Validates a text editor's content and applies thematic coloring.
 * @details Performs a multi-stage check:
 *          1. **Format Check**: Is the string a valid HH:MM:SS.ms pattern?
 *          2. **Range Check**: Is the time between 0.0 and totalLength?
 *          3. **Warning Check**: Is the value near a boundary?
 *          
 *          It then updates `editor.setTextColor()` using `Config::Colors` (e.g., 
 *          `textEditorError` or `textEditorWarning`).
 *
 * @param editor The juce::TextEditor instance to validate and colorize.
 * @param totalLength The maximum allowed duration in seconds.
 * @param sampleRate The audio sample rate for precision validation.
 */
void validateTimeEntry(juce::TextEditor &editor, double totalLength, double sampleRate);

/**
 * @brief Calculates the discrete step size for a given character position in a timestamp.
 * @details Mathematical approach:
 *          - Indices [0-1] (HH): Step = 3600.0s
 *          - Indices [3-4] (MM): Step = 60.0s
 *          - Indices [6-7] (SS): Step = 1.0s
 *          - Indices [9-11] (ms): Step = 0.001s
 *          - Modifiers: Shift = Fine, Alt = Ultra-Fine.
 *
 * @param charIndex The character index of the cursor within the string.
 * @param mods The modifier keys currently held (Shift/Alt/Ctrl).
 * @param sampleRate The sample rate used for sample-accurate fine-stepping.
 * @return The calculated step size in seconds.
 */
double calculateStepSize(int charIndex, const juce::ModifierKeys &mods, double sampleRate = 0.0);

/**
 * @brief Provides a relative zoom factor based on the hovered time segment.
 * @details Used by the 'Zoom Popup' to determine how much detail to show. 
 *          Hovering 'ms' results in a high zoom factor, while 'HH' shows a wide view.
 * @param event The mouse move event capturing the hover position.
 * @return A float factor representing the desired visualization scale.
 */
float getZoomFactorForPosition(const juce::MouseEvent &event);

/** 
 * @brief Determines if the current mouse state warrants displaying a zoom preview.
 * @param event The mouse event to analyze.
 * @return True if a zoom popup should be instantiated.
 */
bool shouldShowZoomPopup(const juce::MouseEvent &event);
} // namespace TimeEntryHelpers

#endif
