#pragma once
#include <JuceHeader.h>
#include <queue>
#include <mutex>

/**
 * @file RoutingWorker.h
 * @ingroup Logic
 * @brief Thread-safe background worker for executing file moves/copies.
 */
struct RoutingJob {
    juce::File sourceFile;
    juce::File destinationDir;
    bool isCopyMode;
};

class RoutingWorker : public juce::Thread {
public:
    RoutingWorker();
    ~RoutingWorker() override;

    /** @brief Enqueues a file operation to be executed off the Message Thread. */
    void enqueueJob(const juce::File& sourceFile, const juce::File& destinationDir, bool isCopyMode);

private:
    void run() override;

    std::queue<RoutingJob> jobQueue;
    std::mutex queueMutex;
    /** @brief Controls the lifetime of async callbacks, ensuring they don't execute after the worker is destroyed. */
    std::shared_ptr<bool> lifeToken;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoutingWorker)
};
