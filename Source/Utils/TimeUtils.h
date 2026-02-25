#ifndef AUDIOFILER_TIMEUTILS_H
#define AUDIOFILER_TIMEUTILS_H

#include <juce_core/juce_core.h>

/**
 * @ingroup Helpers
 * @class TimeUtils
 * @brief Static utility class for time formatting and parsing.
 */
class TimeUtils {
  public:
    /**
     * @brief Formats a time value in seconds into a string (HH:MM:SS:samples).
     * @param seconds The time in seconds.
     * @param sampleRate The sample rate.
     * @return The formatted string.
     */
    static juce::String formatTime(double seconds, double sampleRate);

    /**
     * @brief Parses a time string (e.g., "00:00:01:44100") into seconds.
     * @param timeString The string to parse.
     * @param sampleRate The sample rate.
     * @return The time in seconds, or 0.0 if parsing fails.
     */
    static double parseTime(const juce::String &timeString, double sampleRate);
};

#endif
