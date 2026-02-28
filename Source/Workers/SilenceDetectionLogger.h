#ifndef AUDIOFILER_SILENCEDETECTIONLOGGER_H
#define AUDIOFILER_SILENCEDETECTIONLOGGER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Workers/SilenceWorkerClient.h"

/**
 * @file SilenceDetectionLogger.h
 * @ingroup Helpers
 * @brief Diagnostic static utility for the silence detection engine.
 * 
 * @details Architecturally, SilenceDetectionLogger serves as a "Diagnostic Provider" 
 *          that encapsulates all console and UI-bound messaging related to the 
 *          SilenceAnalysisWorker's lifecycle. It centralizes the construction 
 *          of human-readable status strings, ensuring consistent nomenclature 
 *          across log files and the UI status bar.
 * 
 *          By delegating message formatting to this static class, the 
 *          SilenceAnalysisWorker remains focused on pure mathematical and 
 *          threading logic.
 * 
 * @see SilenceAnalysisWorker
 * @see SilenceWorkerClient
 */
class SilenceDetectionLogger final {
  public:
    /**
     * @brief Logs an error when analysis is triggered without a loaded audio asset.
     * @param client The client that will receive the error message.
     */
    static void logNoAudioLoaded(SilenceWorkerClient &client);

    /**
     * @brief Reports the start of a sample scanning pass.
     * @param client The client to notify.
     * @param direction A string description of the pass (e.g., "Forward," "Reverse").
     * @param length The total number of samples to be scanned.
     */
    static void logReadingSamples(SilenceWorkerClient &client, const juce::String &direction,
                                  juce::int64 length);

    /**
     * @brief Logs an error when a file has a reported length of zero samples.
     * @param client The client to notify.
     */
    static void logZeroLength(SilenceWorkerClient &client);

    /**
     * @brief Logs the successful detection of the 'In' point.
     * @param client The client to notify.
     * @param sampleIndex The absolute sample index where sound begins.
     * @param sampleRate The file's sample rate for time conversion.
     */
    static void logCutInSet(SilenceWorkerClient &client, juce::int64 sampleIndex,
                            double sampleRate);

    /**
     * @brief Logs the successful detection of the 'Out' point.
     * @param client The client to notify.
     * @param sampleIndex The absolute sample index where sound ends.
     * @param sampleRate The file's sample rate for time conversion.
     */
    static void logCutOutSet(SilenceWorkerClient &client, juce::int64 sampleIndex,
                             double sampleRate);

    /**
     * @brief Logs a failure to find sound above the given threshold.
     * @param client The client to notify.
     * @param boundaryDescription Which boundary failed (In or Out).
     */
    static void logNoSoundFound(SilenceWorkerClient &client,
                                const juce::String &boundaryDescription);

    /**
     * @brief Logs a fatal error when an audio file exceeds the supported analysis limits.
     * @param client The client to notify.
     */
    static void logAudioTooLarge(SilenceWorkerClient &client);
};

#endif
