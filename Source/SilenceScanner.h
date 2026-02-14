#ifndef AUDIOFILER_SILENCESCANNER_H
#define AUDIOFILER_SILENCESCANNER_H

#ifdef TESTING_WITHOUT_JUCE
    #include "../Tests/StubJuce.h"
#else
    #include <JuceHeader.h>
#endif

class SilenceScanner
{
public:
    /**
     * @brief Finds the first sample exceeding the threshold in the audio file.
     * @param reader The audio format reader to read samples from.
     * @param threshold The threshold value to detect sound.
     * @return The sample index of the first sound found, or -1 if silence.
     */
    static juce::int64 findFirstSoundSample(juce::AudioFormatReader* reader, float threshold);

    /**
     * @brief Finds the last sample exceeding the threshold in the audio file.
     * @param reader The audio format reader to read samples from.
     * @param threshold The threshold value to detect sound.
     * @return The sample index of the last sound found, or -1 if silence.
     */
    static juce::int64 findLastSoundSample(juce::AudioFormatReader* reader, float threshold);
};

#endif // AUDIOFILER_SILENCESCANNER_H
