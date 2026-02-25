

#include "Core/SilenceAnalysisWorker.h"
#include "Core/FileMetadata.h"
#include "Core/SessionState.h"
#include "Workers/SilenceAnalysisAlgorithms.h"
#include "Workers/SilenceDetectionLogger.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

#include <algorithm>
#include <cmath>
#include <mutex>

SilenceAnalysisWorker::SilenceAnalysisWorker(SilenceWorkerClient &owner, SessionState &state,
                                               juce::AudioFormatManager &fm)
    : Thread("SilenceWorker"), client(owner), sessionState(state), formatManager(fm) {
    lifeToken = std::make_shared<bool>(true);
}

SilenceAnalysisWorker::~SilenceAnalysisWorker() {
    stopThread(4000);
}

bool SilenceAnalysisWorker::isBusy() const {
    return busy.load() || isThreadRunning();
}

void SilenceAnalysisWorker::startAnalysis(float thresholdVal, bool isIn,
                                          const juce::String &filePath) {
    if (isBusy())
        return;

    threshold.store(thresholdVal);
    detectingIn.store(isIn);
    assignedFilePath = filePath;

    startThread();
}

void SilenceAnalysisWorker::run() {
    busy.store(true);

    const juce::String filePath = assignedFilePath;

    juce::File fileToAnalyze(filePath);

    std::unique_ptr<juce::AudioFormatReader> localReader(
        formatManager.createReaderFor(fileToAnalyze));

    juce::int64 result = -1;
    bool success = false;
    juce::int64 sampleRate = 0;
    juce::int64 lengthInSamples = 0;

    if (localReader != nullptr) {
        sampleRate = (juce::int64)localReader->sampleRate;
        lengthInSamples = localReader->lengthInSamples;

        if (detectingIn.load()) {
            result = SilenceAnalysisAlgorithms::findSilenceIn(*localReader, threshold.load(), this);
        } else {
            result =
                SilenceAnalysisAlgorithms::findSilenceOut(*localReader, threshold.load(), this);
        }
        success = true;
    }

    std::weak_ptr<bool> weakToken = lifeToken;

    juce::MessageManager::callAsync(
        [this, weakToken, result, success, sampleRate, lengthInSamples, filePath]() {
            if (auto token = weakToken.lock()) {
                if (!success || lengthInSamples <= 0) {
                    if (lengthInSamples <= 0 && success)
                        client.logStatusMessage(Config::Labels::errorZeroLength, true);
                    else
                        client.logStatusMessage(Config::Labels::errorNoAudio, true);
                } else {
                    client.logStatusMessage(Config::Labels::scanningCutPoints);

                    const bool stillActive = detectingIn.load() ? client.isAutoCutInActive()
                                                                : client.isAutoCutOutActive();

                    FileMetadata metadata = sessionState.getMetadataForFile(filePath);
                    if (result != -1) {
                        const double resultSeconds = (double)result / (double)sampleRate;
                        if (detectingIn.load()) {
                            if (stillActive) {
                                metadata.cutIn = resultSeconds;
                                client.logStatusMessage(
                                    Config::Labels::logStartSet +
                                    juce::String(result) +
                                    Config::Labels::openBracket +
                                    TimeUtils::formatTime((double)result / (double)sampleRate, (double)sampleRate) +
                                    Config::Labels::closeBracket);
                            }
                        } else {
                            const juce::int64 tailSamples = (juce::int64)(sampleRate * 0.05);
                            const juce::int64 endPoint64 = result + tailSamples;
                            const juce::int64 finalEndPoint = std::min(endPoint64, lengthInSamples);
                            const double endSeconds = (double)finalEndPoint / (double)sampleRate;

                            if (stillActive) {
                                metadata.cutOut = endSeconds;
                                client.logStatusMessage(
                                    Config::Labels::logEndSet +
                                    juce::String(finalEndPoint) +
                                    Config::Labels::openBracket +
                                    TimeUtils::formatTime((double)finalEndPoint / (double)sampleRate, (double)sampleRate) +
                                    Config::Labels::closeBracket);
                            }
                        }
                    } else {
                        client.logStatusMessage(Config::Labels::noSilenceBoundaries);
                    }

                    if (stillActive) {
                        metadata.isAnalyzed = true;
                        sessionState.setMetadataForFile(filePath, metadata);
                    }
                }

                busy.store(false);
            }
        });
}
