#ifndef AUDIOFILER_SILENCEANALYSISWORKER_H
#define AUDIOFILER_SILENCEANALYSISWORKER_H

#include <JuceHeader.h>
#include <functional>

/**
 * @class SilenceAnalysisWorker
 * @brief Handles the heavy lifting for detecting leading/trailing silence asynchronously.
 *
 * This class scans audio in chunks using a timer to prevent blocking the message thread.
 * It is decoupled from UI components and reports results via callbacks.
 */
class SilenceAnalysisWorker : public juce::Timer
{
public:
    SilenceAnalysisWorker();
    ~SilenceAnalysisWorker() override;

    //==============================================================================
    // Callbacks to report results

    /** Called when silence boundary is found. Arguments: sampleIndex, sampleRate */
    std::function<void(juce::int64, double)> onSilenceFound;

    /** Called when scan completes without finding sound. Argument: description (e.g. "start", "end") */
    std::function<void(juce::String)> onNoSilenceFound;

    /** Called when an error occurs (e.g. invalid file). Argument: error message */
    std::function<void(juce::String)> onError;

    /** Called periodically or on events for logging. Argument: log message */
    std::function<void(juce::String)> onLog;

    //==============================================================================

    /**
     * @brief Starts silence-in detection.
     * @param reader The audio reader to scan. Must remain valid during the scan.
     * @param threshold Normalized threshold (0-1) for detecting sound.
     */
    void startInDetection(juce::AudioFormatReader* reader, float threshold);

    /**
     * @brief Starts silence-out detection.
     * @param reader The audio reader to scan. Must remain valid during the scan.
     * @param threshold Normalized threshold (0-1) for detecting sound.
     */
    void startOutDetection(juce::AudioFormatReader* reader, float threshold);

    /** Stops any ongoing analysis. */
    void stop();

    /** Returns true if analysis is running. */
    bool isWorking() const { return isTimerRunning(); }

    // Exposed for testing/benchmarking to manually pump the loop
    void processNextChunk();

private:
    void timerCallback() override;

    juce::AudioFormatReader* currentReader = nullptr;
    float currentThreshold = 0.0f;
    bool isDetectingIn = true;

    juce::int64 currentPos = 0;
    juce::int64 lengthInSamples = 0;

    // Buffer for chunk processing
    juce::AudioBuffer<float> buffer;

    // Constants
    static constexpr int kChunkSize = 65536;
    static constexpr juce::int64 kMaxAnalyzableSamples = 2147483647; // INT_MAX
    static constexpr int kMaxChannels = 128;
};

#endif // AUDIOFILER_SILENCEANALYSISWORKER_H
