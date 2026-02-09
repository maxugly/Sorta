#include "SilenceAnalysisWorker.h"

#include "ControlPanel.h"
#include "AudioPlayer.h"

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
        ownerPanel.getStatsDisplay().insertTextAtCaret("No audio loaded to detect silence.\n");
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    const juce::int64 lengthInSamples = reader->lengthInSamples;
    ownerPanel.getStatsDisplay().insertTextAtCaret("SilenceDetector: Reading " + juce::String(lengthInSamples) + " samples for In detection.\n");
    if (lengthInSamples == 0)
    {
        ownerPanel.getStatsDisplay().insertTextAtCaret("SilenceDetector: Audio length is 0, cannot detect silence.\n");
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    auto buffer = std::make_unique<juce::AudioBuffer<float>>(reader->numChannels, (int) lengthInSamples);
    reader->read(buffer.get(), 0, (int) lengthInSamples, 0, true, true);

    for (int sample = 0; sample < buffer->getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < buffer->getNumChannels(); ++channel)
        {
            if (std::abs(buffer->getSample(channel, sample)) > threshold)
            {
                ownerPanel.setLoopStart(sample);
                ownerPanel.getStatsDisplay().insertTextAtCaret("Auto-set loop start to sample " + juce::String(sample) + " (" + ownerPanel.formatTime((double) sample / reader->sampleRate) + ")\n");
                resumeIfNeeded(audioPlayer, wasPlaying);
                return;
            }
        }
    }

    ownerPanel.getStatsDisplay().insertTextAtCaret("Could not detect any sound at start.\n");
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
        ownerPanel.getStatsDisplay().insertTextAtCaret("No audio loaded to detect silence.\n");
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    const juce::int64 lengthInSamples = reader->lengthInSamples;
    ownerPanel.getStatsDisplay().insertTextAtCaret("SilenceDetector: Reading " + juce::String(lengthInSamples) + " samples for Out detection.\n");
    if (lengthInSamples == 0)
    {
        ownerPanel.getStatsDisplay().insertTextAtCaret("SilenceDetector: Audio length is 0, cannot detect silence.\n");
        resumeIfNeeded(audioPlayer, wasPlaying);
        return;
    }

    auto buffer = std::make_unique<juce::AudioBuffer<float>>(reader->numChannels, (int) lengthInSamples);
    reader->read(buffer.get(), 0, (int) lengthInSamples, 0, true, true);

    for (int sample = buffer->getNumSamples() - 1; sample >= 0; --sample)
    {
        for (int channel = 0; channel < buffer->getNumChannels(); ++channel)
        {
            if (std::abs(buffer->getSample(channel, sample)) > threshold)
            {
                int endPoint = sample + (int) (reader->sampleRate * 0.05); // 50ms tail
                endPoint = juce::jmin(endPoint, buffer->getNumSamples());
                ownerPanel.setLoopEnd(endPoint);
                ownerPanel.getStatsDisplay().insertTextAtCaret("Auto-set loop end to sample " + juce::String(endPoint) + " (" + ownerPanel.formatTime((double) endPoint / reader->sampleRate) + ")\n");
                resumeIfNeeded(audioPlayer, wasPlaying);
                return;
            }
        }
    }

    ownerPanel.getStatsDisplay().insertTextAtCaret("Could not detect any sound at end.\n");
    resumeIfNeeded(audioPlayer, wasPlaying);
}
