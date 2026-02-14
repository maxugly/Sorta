#include "SilenceAnalysisWorker.h"
#include "SilenceScanner.h"

#include "ControlPanel.h"
#include "AudioPlayer.h"
#include "SilenceDetectionLogger.h"
#include <limits>

namespace
{
    void resumeIfNeeded(AudioPlayer& player, bool wasPlaying)
    {
        if (wasPlaying)
            player.getTransportSource().start();
    }
}

void SilenceAnalysisWorker::detectInSilence(ControlPanel& ownerPanel, float threshold)
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
    SilenceDetectionLogger::logReadingSamples(ownerPanel, "In", lengthInSamples);
    if (lengthInSamples == 0)
    {
        SilenceDetectionLogger::logZeroLength(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    // Keep this check to protect downstream int-based APIs
    if (lengthInSamples > std::numeric_limits<int>::max())
    {
        SilenceDetectionLogger::logAudioTooLarge(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    juce::int64 sample = SilenceScanner::findFirstSoundSample(reader, threshold);

    if (sample >= 0)
    {
        ownerPanel.setLoopStart((int)sample);
        SilenceDetectionLogger::logLoopStartSet(ownerPanel, (int)sample, reader->sampleRate);
        // Move playhead to the new loop-in position in cut mode
        if (ownerPanel.isCutModeActive())
            audioPlayer.getTransportSource().setPosition(ownerPanel.getLoopInPosition());
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    SilenceDetectionLogger::logNoSoundFound(ownerPanel, "start");
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
    if (lengthInSamples == 0)
    {
        SilenceDetectionLogger::logZeroLength(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    // Keep this check to protect downstream int-based APIs
    if (lengthInSamples > std::numeric_limits<int>::max())
    {
        SilenceDetectionLogger::logAudioTooLarge(ownerPanel);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    juce::int64 sample = SilenceScanner::findLastSoundSample(reader, threshold);

    if (sample >= 0)
    {
        const juce::int64 tailSamples = (juce::int64) (reader->sampleRate * 0.05); // 50ms tail
        const juce::int64 endPoint64 = sample + tailSamples;
        const int endPoint = (int) juce::jmin (endPoint64, lengthInSamples);

        ownerPanel.setLoopEnd(endPoint);
        SilenceDetectionLogger::logLoopEndSet(ownerPanel, endPoint, reader->sampleRate);
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    SilenceDetectionLogger::logNoSoundFound(ownerPanel, "end");
    resumeIfNeeded(audioPlayer, wasPlaying);
}
