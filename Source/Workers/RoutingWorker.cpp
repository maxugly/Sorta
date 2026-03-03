#include "Workers/RoutingWorker.h"

RoutingWorker::RoutingWorker() : Thread("RoutingWorker") {
    lifeToken = std::make_shared<bool>(true);
}

RoutingWorker::~RoutingWorker() {
    stopThread(4000);
}

void RoutingWorker::enqueueJob(const juce::File& sourceFile, const juce::File& destinationDir, bool isCopyMode) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        jobQueue.push({sourceFile, destinationDir, isCopyMode});
    }
    startThread();
}

void RoutingWorker::run() {
    while (!threadShouldExit()) {
        RoutingJob job;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (jobQueue.empty()) break;
            job = jobQueue.front();
            jobQueue.pop();
        }

        if (!job.destinationDir.exists()) {
            job.destinationDir.createDirectory();
        }

        juce::File targetFile = job.destinationDir.getChildFile(job.sourceFile.getFileName());
        bool success = false;

        if (job.isCopyMode) {
            success = job.sourceFile.copyFileTo(targetFile);
        } else {
            success = job.sourceFile.moveFileTo(targetFile);
        }

        // Return safely via Async
        std::weak_ptr<bool> weakToken = lifeToken;
        juce::MessageManager::callAsync([weakToken, success, targetFile]() {
            if (weakToken.lock()) {
                // Future expansion: Push success/fail to HintView here
                juce::ignoreUnused(success, targetFile);
            }
        });
    }
}
