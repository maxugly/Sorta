#include "SilenceScanner.h"
#include <cmath>
#include <algorithm>

juce::int64 SilenceScanner::findFirstSoundSample(juce::AudioFormatReader* reader, float threshold)
{
    if (reader == nullptr || reader->lengthInSamples == 0)
        return -1;

    const int chunkSize = 65536;
    const int numChannels = (int) reader->numChannels;
    juce::AudioBuffer<float> buffer(numChannels, chunkSize);

    juce::int64 currentSample = 0;
    const juce::int64 totalSamples = reader->lengthInSamples;

    while (currentSample < totalSamples)
    {
        buffer.clear();
        const juce::int64 samplesRemaining = totalSamples - currentSample;
        const int samplesToRead = (int) std::min((juce::int64) chunkSize, samplesRemaining);

        reader->read(&buffer, 0, samplesToRead, currentSample, true, true);

        for (int i = 0; i < samplesToRead; ++i)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                if (std::abs(buffer.getSample(channel, i)) > threshold)
                {
                    return currentSample + i;
                }
            }
        }

        currentSample += samplesToRead;
    }

    return -1;
}

juce::int64 SilenceScanner::findLastSoundSample(juce::AudioFormatReader* reader, float threshold)
{
    if (reader == nullptr || reader->lengthInSamples == 0)
        return -1;

    const int chunkSize = 65536;
    const int numChannels = (int) reader->numChannels;
    juce::AudioBuffer<float> buffer(numChannels, chunkSize);

    const juce::int64 totalSamples = reader->lengthInSamples;
    juce::int64 currentEndSample = totalSamples;

    while (currentEndSample > 0)
    {
        buffer.clear();
        const juce::int64 samplesRemaining = currentEndSample;
        const int samplesToRead = (int) std::min((juce::int64) chunkSize, samplesRemaining);
        const juce::int64 startSample = currentEndSample - samplesToRead;

        reader->read(&buffer, 0, samplesToRead, startSample, true, true);

        for (int i = samplesToRead - 1; i >= 0; --i)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                if (std::abs(buffer.getSample(channel, i)) > threshold)
                {
                    return startSample + i;
                }
            }
        }

        currentEndSample -= samplesToRead;
    }

    return -1;
}
