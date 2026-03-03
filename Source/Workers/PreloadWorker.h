#pragma once
#include <JuceHeader.h>
#include <queue>
#include <mutex>
#include "Core/FileMetadata.h"

class SessionState;

/**
 * @file PreloadWorker.h
 * @ingroup AudioEngine
 * @brief Thread-safe background worker for proactively analyzing upcoming audio files.
 *
 * @details Implements the "Air Gap" protocol (Contract 3). This worker maintains
 * a queue of file paths. It processes them sequentially on a low-priority thread,
 * instantiating a private AudioFormatReader for each. Results are pushed to the
 * SessionState::metadataCache via juce::MessageManager::callAsync.
 */
class PreloadWorker : public juce::Thread {
public:
    PreloadWorker(SessionState& sessionState, juce::AudioFormatManager& formatManager);
    ~PreloadWorker() override;

    /** @brief Adds a file to the background analysis queue. */
    void enqueueFile(const juce::String& filePath);

    /** @brief Clears the queue and stops current analysis. */
    void clearQueue();

private:
    void run() override;

    SessionState& sessionState;
    juce::AudioFormatManager& formatManager;

    std::queue<juce::String> fileQueue;
    std::mutex queueMutex;
    std::shared_ptr<bool> lifeToken;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreloadWorker)
};
