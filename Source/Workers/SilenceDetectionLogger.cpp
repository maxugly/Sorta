

#include "Workers/SilenceDetectionLogger.h"
#include "Utils/TimeUtils.h"
#include "Utils/Config.h"

void SilenceDetectionLogger::logNoAudioLoaded(SilenceWorkerClient &client) {
    client.logStatusMessage(Config::Labels::logNoAudio, true);
}

void SilenceDetectionLogger::logReadingSamples(SilenceWorkerClient &client,
                                               const juce::String &direction, juce::int64 length) {
    client.logStatusMessage(Config::Labels::logScanning + juce::String(length) + Config::Labels::logSamplesFor +
                            direction + Config::Labels::logBoundary);
}

void SilenceDetectionLogger::logZeroLength(SilenceWorkerClient &client) {
    client.logStatusMessage(Config::Labels::logZeroLength,
                            true);
}

void SilenceDetectionLogger::logCutInSet(SilenceWorkerClient &client, juce::int64 sampleIndex,
                                         double sampleRate) {
    client.logStatusMessage(Config::Labels::logStartSet + juce::String(sampleIndex) +
                            Config::Labels::openBracket + TimeUtils::formatTime((double)sampleIndex / sampleRate) + Config::Labels::closeBracket);
}

void SilenceDetectionLogger::logCutOutSet(SilenceWorkerClient &client, juce::int64 sampleIndex,
                                          double sampleRate) {
    client.logStatusMessage(Config::Labels::logEndSet + juce::String(sampleIndex) +
                            Config::Labels::openBracket + TimeUtils::formatTime((double)sampleIndex / sampleRate) + Config::Labels::closeBracket);
}

void SilenceDetectionLogger::logNoSoundFound(SilenceWorkerClient &client,
                                             const juce::String &boundaryDescription) {
    client.logStatusMessage(Config::Labels::logNoSound + boundaryDescription +
                            Config::Labels::logBoundary);
}

void SilenceDetectionLogger::logAudioTooLarge(SilenceWorkerClient &client) {
    client.logStatusMessage(
        Config::Labels::logTooLarge, true);
}
