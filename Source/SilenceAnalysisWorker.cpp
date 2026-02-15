#include "SilenceAnalysisWorker.h"
#include "SilenceAnalysisAlgorithms.h"

#include "ControlPanel.h"
#include "AudioPlayer.h"
#include "SilenceDetectionLogger.h"
#include <limits>
#include <algorithm>
#include <cmath>

SilenceAnalysisWorker::SilenceAnalysisWorker()
{
    void resumeIfNeeded(AudioPlayer& player, bool wasPlaying)
    {
        if (wasPlaying)
            player.getTransportSource().start();
    }
}

SilenceAnalysisWorker::~SilenceAnalysisWorker()
{
    stop();
}

void SilenceAnalysisWorker::stop()
{
    stopTimer();
    currentReader = nullptr;
}

void SilenceAnalysisWorker::startInDetection(juce::AudioFormatReader* reader, float threshold)
{
    stop();
    if (reader == nullptr) return;

    currentReader = reader;
    currentThreshold = threshold;
    isDetectingIn = true;
    lengthInSamples = reader->lengthInSamples;

    // Validate
    if (lengthInSamples <= 0)
    {
        if (onError) onError("SilenceDetector: Audio length is 0, cannot detect silence.");
        return;
    }

    if (lengthInSamples > kMaxAnalyzableSamples)
    {
        if (onError) onError("SilenceDetector: Audio file is too large for automated silence detection.");
        return;
    }

    if (reader->numChannels <= 0 || reader->numChannels > kMaxChannels)
    {
        if (onError) onError("SilenceDetector: Invalid number of channels (" + juce::String(reader->numChannels) + ")");
        return;
    }

    if (onLog) onLog("SilenceDetector: Reading " + juce::String(lengthInSamples) + " samples for In detection.");

    // Setup buffer (resizing only if needed)
    buffer.setSize(reader->numChannels, kChunkSize);

    currentPos = 0;
    startTimer(1); // Start immediately with 1ms interval
}

void SilenceAnalysisWorker::startOutDetection(juce::AudioFormatReader* reader, float threshold)
{
    stop();
    if (reader == nullptr) return;

    currentReader = reader;
    currentThreshold = threshold;
    isDetectingIn = false;
    lengthInSamples = reader->lengthInSamples;

    // Validate
    if (lengthInSamples <= 0)
    {
        if (onError) onError("SilenceDetector: Audio length is 0, cannot detect silence.");
        return;
    }

    // Note: DoS check for kMaxAnalyzableSamples removed as chunking handles large files safely

    const juce::int64 result = SilenceAnalysisAlgorithms::findSilenceIn(*reader, threshold);

    if (result != -1)
    {
        ownerPanel.setLoopInPosition((double)result / reader->sampleRate);
        SilenceDetectionLogger::logLoopStartSet(ownerPanel, result, reader->sampleRate);

        // Move playhead to the new loop-in position in cut mode
        if (ownerPanel.isCutModeActive())
            audioPlayer.getTransportSource().setPosition(ownerPanel.getLoopInPosition());
    }
    else
    {
        SilenceDetectionLogger::logNoSoundFound(ownerPanel, "start");
    }

    resumeIfNeeded(audioPlayer, wasPlaying);
}

void SilenceAnalysisWorker::detectOutSilence(ControlPanel& ownerPanel, float threshold)
{
    AudioPlayer& audioPlayer = ownerPanel.getAudioPlayer();
    const bool wasPlaying = audioPlayer.isPlaying();
    if (wasPlaying)
        audioPlayer.getTransportSource().stop();

    juce::AudioFormatReader* reader = audioPlayer.getAudioFormatReader();
    if (reader == nullptr)
    {
        SilenceDetectionLogger::logNoAudioLoaded(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    const juce::int64 lengthInSamples = reader->lengthInSamples;
    SilenceDetectionLogger::logReadingSamples(ownerPanel, "Out", lengthInSamples);

    // Security Fix: Check for invalid length
    if (lengthInSamples <= 0)
    {
        SilenceDetectionLogger::logZeroLength(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    // Note: DoS check for kMaxAnalyzableSamples removed as chunking handles large files safely

    const juce::int64 result = SilenceAnalysisAlgorithms::findSilenceOut(*reader, threshold);

    if (result != -1)
    {
        const juce::int64 tailSamples = (juce::int64) (reader->sampleRate * 0.05); // 50ms tail
        const juce::int64 endPoint64 = result + tailSamples;
        const juce::int64 finalEndPoint = std::min(endPoint64, lengthInSamples);

        ownerPanel.setLoopOutPosition((double)finalEndPoint / reader->sampleRate);
        SilenceDetectionLogger::logLoopEndSet(ownerPanel, finalEndPoint, reader->sampleRate);
    }
    else
    {
        SilenceDetectionLogger::logNoSoundFound(ownerPanel, "end");
    }

    resumeIfNeeded(audioPlayer, wasPlaying);
    if (lengthInSamples > kMaxAnalyzableSamples)
    {
        if (onError) onError("SilenceDetector: Audio file is too large for automated silence detection.");
        return;
    }

    if (reader->numChannels <= 0 || reader->numChannels > kMaxChannels)
    {
        if (onError) onError("SilenceDetector: Invalid number of channels (" + juce::String(reader->numChannels) + ")");
        return;
    }

    if (onLog) onLog("SilenceDetector: Reading " + juce::String(lengthInSamples) + " samples for Out detection.");

    buffer.setSize(reader->numChannels, kChunkSize);
    currentPos = lengthInSamples;
    startTimer(1);
}

void SilenceAnalysisWorker::timerCallback()
{
    processNextChunk();
}

void SilenceAnalysisWorker::processNextChunk()
{
    if (currentReader == nullptr)
    {
        stop();
        return;
    }

    if (isDetectingIn)
    {
        // IN Detection
        if (currentPos >= lengthInSamples)
        {
            if (onNoSilenceFound) onNoSilenceFound("start");
            stop();
            return;
        }

        const int numThisTime = (int) std::min((juce::int64) kChunkSize, lengthInSamples - currentPos);
        buffer.clear();

        if (!currentReader->read(&buffer, 0, numThisTime, currentPos, true, true))
        {
            if (onError) onError("SilenceDetector: Failed to read audio.");
            stop();
            return;
        }

        for (int sample = 0; sample < numThisTime; ++sample)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                if (std::abs(buffer.getSample(channel, sample)) > currentThreshold)
                {
                    const juce::int64 globalSample = currentPos + sample;
                    if (onSilenceFound) onSilenceFound(globalSample, currentReader->sampleRate);
                    stop();
                    return;
                }
            }
        }
        currentPos += numThisTime;
    }
    else
    {
        // OUT Detection
        if (currentPos <= 0)
        {
            if (onNoSilenceFound) onNoSilenceFound("end");
            stop();
            return;
        }

        const int numThisTime = (int) std::min((juce::int64) kChunkSize, currentPos);
        const juce::int64 startSample = currentPos - numThisTime;

        buffer.clear();
        if (!currentReader->read(&buffer, 0, numThisTime, startSample, true, true))
        {
            if (onError) onError("SilenceDetector: Failed to read audio.");
            stop();
            return;
        }

        for (int sample = numThisTime - 1; sample >= 0; --sample)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                if (std::abs(buffer.getSample(channel, sample)) > currentThreshold)
                {
                    const juce::int64 globalSample = startSample + sample;
                    const juce::int64 tailSamples = (juce::int64) (currentReader->sampleRate * 0.05); // 50ms tail
                    const juce::int64 endPoint64 = globalSample + tailSamples;
                    const juce::int64 finalEndPoint = std::min(endPoint64, lengthInSamples);

                    if (onSilenceFound) onSilenceFound(finalEndPoint, currentReader->sampleRate);
                    stop();
                    return;
                }
            }
        }
        currentPos -= numThisTime;
    }
}
