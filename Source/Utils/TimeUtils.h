#ifndef AUDIOFILER_TIMEUTILS_H
#define AUDIOFILER_TIMEUTILS_H

#include <juce_core/juce_core.h>

/**
 * @file TimeUtils.h
 * @ingroup Helpers
 * @brief Static "Pure Logic Engine" for bidirectional time translation.
 * 
 * @details Architecturally, TimeUtils serves as the "Universal Translator" for 
 *          temporal data within the application. It handles the critical 
 *          task of converting high-precision floating-point seconds (used 
 *          by the audio engine) into human-readable strings (used by the UI) 
 *          and vice-versa.
 * 
 *          The class follows a "Functional Utility" pattern—all methods are 
 *          stateless and thread-safe, allowing them to be called from both 
 *          the Message Thread (for UI formatting) and background workers.
 * 
 * @see TimeEntryHelpers
 * @see SessionState
 */
class TimeUtils {
  public:
    /**
     * @brief Formats a precise duration in seconds into a standard HH:MM:SS:ms string.
     * @details Mathematical approach:
     *          - Hours: `floor(seconds / 3600)`
     *          - Minutes: `floor((seconds % 3600) / 60)`
     *          - Seconds: `floor(seconds % 60)`
     *          - Milliseconds: `(seconds - totalSeconds) * 1000`
     * 
     * @param seconds The absolute duration in seconds to format.
     * @param sampleRate The sample rate used to ensure sub-millisecond precision 
     *                   is accurately represented.
     * @return A formatted juce::String (e.g., "00:01:45.123").
     */
    static juce::String formatTime(double seconds, double sampleRate);

    /**
     * @brief Parses a human-entered time string back into a floating-point seconds value.
     * @details This method is robust, handling various separators and incomplete strings. 
     *          It expects the pattern `[HH:]MM:SS[.ms]`.
     * 
     * @param timeString The string to attempt to parse.
     * @param sampleRate The sample rate used to validate and scale the output.
     * @return The absolute time in seconds, or 0.0 if the string is completely malformed.
     */
    static double parseTime(const juce::String &timeString, double sampleRate);
};

#endif
