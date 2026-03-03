#include "Workers/PreloadWorker.h"
#include "Core/SessionState.h"
#include "Workers/SilenceAnalysisAlgorithms.h"
#include "Utils/Config.h"

PreloadWorker::PreloadWorker(SessionState& state, juce::AudioFormatManager& fm)
    : Thread("PreloadWorker"), sessionState(state), formatManager(fm) {
    lifeToken = std::make_shared<bool>(true);
}

PreloadWorker::~PreloadWorker() {
    stopThread(4000);
}

void PreloadWorker::enqueueFile(const juce::String& filePath) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        fileQueue.push(filePath);
    }
    startThread();
}

void PreloadWorker::clearQueue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::queue<juce::String> empty;
    std::swap(fileQueue, empty);
}

void PreloadWorker::run() {
    while (!threadShouldExit()) {
        juce::String nextFile;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (fileQueue.empty()) break;
            nextFile = fileQueue.front();
            fileQueue.pop();
        }

        // 1. Skip if already analyzed in cache
        if (sessionState.hasMetadataForFile(nextFile) && sessionState.getMetadataForFile(nextFile).isAnalyzed) {
            continue;
        }

        // 2. Air Gap: Private Reader
        juce::File fileToAnalyze(nextFile);
        std::unique_ptr<juce::AudioFormatReader> localReader(formatManager.createReaderFor(fileToAnalyze));

        if (localReader != nullptr) {
            // Grab user preferences from state
            const float threshIn = sessionState.getCutPrefs().autoCut.thresholdIn;
            const float threshOut = sessionState.getCutPrefs().autoCut.thresholdOut;
            const double sampleRate = localReader->sampleRate;

            // 3. Execute Math
            juce::int64 inResult = SilenceAnalysisAlgorithms::findSilenceIn(*localReader, threshIn, this);
            juce::int64 outResult = SilenceAnalysisAlgorithms::findSilenceOut(*localReader, threshOut, this);

            FileMetadata metadata;
            metadata.isAnalyzed = true;
            if (inResult != -1 && sampleRate > 0) metadata.cutIn = (double)inResult / sampleRate;
            if (outResult != -1 && sampleRate > 0) metadata.cutOut = (double)outResult / sampleRate;
            else if (sampleRate > 0) metadata.cutOut = (double)localReader->lengthInSamples / sampleRate;

            // 4. Return via Async Message Thread
            std::weak_ptr<bool> weakToken = lifeToken;
            juce::MessageManager::callAsync([this, weakToken, nextFile, metadata]() {
                if (weakToken.lock()) {
                    sessionState.setMetadataForFile(nextFile, metadata);
                }
            });
        }
    }
}
