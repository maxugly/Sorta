

#ifndef AUDIOFILER_SILENCEANALYSISWORKER_H
#define AUDIOFILER_SILENCEANALYSISWORKER_H

#if defined(JUCE_HEADLESS)
    #include <juce_core/juce_core.h>
    #include <juce_events/juce_events.h>
#else
    #include <JuceHeader.h>
#endif

#include <atomic>
#include <memory>
#include "SilenceWorkerClient.h"

class SessionState;

/**
 * @ingroup Workers
 * @class SilenceAnalysisWorker
 * @brief A background thread for analyzing audio files for silence.
 * @details This worker prevents UI freezing by processing large audio files asynchronously.
 * It communicates results back to the main thread via `SilenceWorkerClient` callbacks.
 * @see SilenceWorkerClient
 */
class SilenceAnalysisWorker : public juce::Thread
{
public:

    explicit SilenceAnalysisWorker(SilenceWorkerClient& client, SessionState& sessionState);

    ~SilenceAnalysisWorker() override;

    /**
     * @brief Triggers a new analysis job.
     * @param threshold The amplitude threshold (0.0 to 1.0) for silence detection.
     * @param detectingIn True to find the start of audio (In point), False for the end (Out point).
     */
    void startAnalysis(float threshold, bool detectingIn);

    bool isBusy() const;

private:

    /**
     * @brief The main thread loop.
     * @details Periodically checks for new analysis jobs. When a job is found,
     * it scans the audio file using `SilenceAnalysisAlgorithms` and posts the result.
     */
    void run() override;

    SilenceWorkerClient& client;
    SessionState& sessionState;
    /** @brief Thread-safe storage for the current analysis threshold. */
    std::atomic<float> threshold { 0.0f };
    /** @brief Flag indicating whether we are searching for the Cut In or Cut Out point. */
    std::atomic<bool> detectingIn { true };
    std::atomic<bool> busy { false };
    bool wasPlayingBeforeScan = false;
    juce::String assignedFilePath;

    std::shared_ptr<bool> lifeToken;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilenceAnalysisWorker)
};

#endif 
