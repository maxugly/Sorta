#ifndef AUDIOFILER_SILENCEANALYSISWORKER_H
#define AUDIOFILER_SILENCEANALYSISWORKER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_formats/juce_audio_formats.h>
#else
#include <JuceHeader.h>
#endif

#include "Workers/SilenceWorkerClient.h"
#include <atomic>
#include <memory>

class SessionState;

/**
 * @file SilenceAnalysisWorker.h
 * @ingroup AudioEngine
 * @brief Thread-safe background worker for non-blocking audio silence detection.
 * 
 * @details Architecturally, this class serves as a "Thread-Safe Background Worker" that 
 *          implements the "Air Gap" protocol. It offloads the computationally expensive 
 *          task of scanning entire audio files for silence regions to a dedicated 
 *          low-priority background thread. This ensures that the UI remains responsive 
 *          and the Audio Thread remains jitter-free during analysis.
 * 
 *          The worker operates by:
 *          1. Creating a private `juce::AudioFormatReader` for the target file, 
 *             ensuring no resource contention with the `AudioPlayer`.
 *          2. Iterating through the audio samples in blocks to identify amplitude 
 *             crossings relative to a user-defined decibel threshold.
 *          3. Packaging the results into a `FileMetadata` object.
 *          4. Communicating results back to the Message Thread via 
 *             `juce::MessageManager::callAsync`, strictly adhering to the threading law.
 * 
 * @see SilenceAnalysisAlgorithms
 * @see SilenceWorkerClient
 * @see SessionState
 * @see FileMetadata
 */
class SilenceAnalysisWorker : public juce::Thread {
  public:
    /**
     * @brief Constructs the worker and initializes its dependencies.
     * @param client Reference to the client interface that will receive results.
     * @param sessionState Reference to the global state for boundary updates.
     * @param formatManager Reference to the format manager for file decoding.
     */
    explicit SilenceAnalysisWorker(SilenceWorkerClient &client, SessionState &sessionState,
                                   juce::AudioFormatManager &formatManager);

    /**
     * @brief Ensures the thread is safely stopped before destruction.
     */
    ~SilenceAnalysisWorker() override;

    /**
     * @brief Triggers a new analysis pass for a specific file.
     * @param threshold The dB threshold to use for silence detection.
     * @param detectingIn True if searching for the 'In' point, false for 'Out'.
     * @param filePath Absolute path to the file to analyze.
     * @note If the worker is already busy, this request may be queued or ignored 
     *       depending on the implementation of the specific algorithm.
     */
    void startAnalysis(float threshold, bool detectingIn, const juce::String &filePath);

    /**
     * @brief Checks if the worker thread is currently executing an analysis pass.
     * @return True if the thread is active.
     */
    bool isBusy() const;

    /**
     * @brief Queries the current direction of detection.
     * @return True if searching for start-of-audio silence.
     */
    bool isDetectingIn() const {
        return detectingIn.load();
    }

  private:
    /**
     * @brief The main execution loop for the background thread.
     * @details This method performs the actual file I/O and sample scanning. 
     *          It periodically checks `threadShouldExit()` to allow for 
     *          immediate cancellation.
     */
    void run() override;

    SilenceWorkerClient &client;                      /**< Interface for pushing results back to the UI. */
    SessionState &sessionState;                        /**< The central state hub for metadata storage. */
    juce::AudioFormatManager &formatManager;          /**< Used to instantiate the private file reader. */
    std::atomic<float> threshold{0.0f};                /**< The dB level considered "silence" for this pass. */
    std::atomic<bool> detectingIn{true};              /**< Directional flag for the analysis algorithm. */
    std::atomic<bool> busy{false};                     /**< Atomic flag indicating background activity. */
    juce::String assignedFilePath;                    /**< The path currently being analyzed. */

    std::shared_ptr<bool> lifeToken;                  /**< Safety token for async callback validation. */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilenceAnalysisWorker)
};

#endif
