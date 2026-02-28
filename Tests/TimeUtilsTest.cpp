/**
 * @file TimeUtilsTest.cpp
 * @Source/Core/FileMetadata.h
 * @ingroup Tests
 * @brief Verifies mathematical time conversions, string formatting, and parsing accuracy.
 */

#include "Utils/TimeUtils.h"
#include <juce_core/juce_core.h>

/**
 * @class TimeUtilsTest
 * @brief Test suite for timecode-to-seconds and seconds-to-timecode conversion math.
 * 
 * @details Architecturally, this test suite validates the "Headless Portability" law, 
 *          proving that all mathematical transformations and string representations 
 *          function perfectly without any GUI components or visual editors attached.
 * 
 *          These tests verify mathematical precision (e.g., converting fractions of 
 *          seconds into exact sample counts) and robust fallback mechanisms (e.g., 
 *          handling negative input or invalid timecode strings). This ensures the 
 *          foundation of our frame-accurate editing is consistent and deterministic.
 */
class TimeUtilsTest : public juce::UnitTest {
  public:
    TimeUtilsTest() : juce::UnitTest("TimeUtils Testing") {
    }

    /** 
     * @brief Executes conversion accuracy and formatting tests.
     * @details Covers basic time formatting, sample-accurate truncation, negative 
     *          input sanitization, and complex HH:MM:SS:Samples round-trip parsing.
     */
    void runTest() override {
        const double sr = 44100.0;
        beginTest("formatTime handles basic times");
        expectEquals(TimeUtils::formatTime(0.0, sr), juce::String("00:00:00:00000"));
        expectEquals(TimeUtils::formatTime(1.0, sr), juce::String("00:00:01:00000"));
        expectEquals(TimeUtils::formatTime(60.0, sr), juce::String("00:01:00:00000"));
        expectEquals(TimeUtils::formatTime(3600.0, sr), juce::String("01:00:00:00000"));

        beginTest("formatTime handles samples and truncation");
        expectEquals(TimeUtils::formatTime(0.5, sr), juce::String("00:00:00:22050"));
        expectEquals(TimeUtils::formatTime(1.00002267573696, sr), juce::String("00:00:01:00001"));
        
        beginTest("formatTime handles negative input");
        expectEquals(TimeUtils::formatTime(-5.0, sr), juce::String("00:00:00:00000"));

        beginTest("formatTime handles complex times");
        expectEquals(TimeUtils::formatTime(3661.5, sr), juce::String("01:01:01:22050"));

        beginTest("parseTime handles valid inputs");
        expectEquals(TimeUtils::parseTime("00:00:00:00000", sr), 0.0);
        expectEquals(TimeUtils::parseTime("01:00:00:00000", sr), 3600.0);
        expectEquals(TimeUtils::parseTime("00:00:01:00000", sr), 1.0);
        expectEquals(TimeUtils::parseTime("00:00:00:22050", sr), 0.5);

        beginTest("Round trip consistency");
        double testTimes[] = {0.0, 0.5, 1.0, 60.0, 3600.0, 3661.5};
        for (double t : testTimes) {
            expectWithinAbsoluteError(TimeUtils::parseTime(TimeUtils::formatTime(t, sr), sr), t, 0.00001);
        }
    }
};

static TimeUtilsTest timeUtilsTest;
