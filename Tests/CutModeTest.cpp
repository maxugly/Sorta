#include <juce_core/juce_core.h>
#include "../Source/AudioPlayer.h"
#include "../Source/SessionState.h"
#include "../Source/PlaybackHelpers.h"

// Mock AudioFormatReader for headless testing
class MockAudioFormatReader : public juce::AudioFormatReader
{
public:
    MockAudioFormatReader() : juce::AudioFormatReader(nullptr, "Mock")
    {
        sampleRate = 44100.0;
        bitsPerSample = 16;
        lengthInSamples = 44100 * 10; // 10 seconds
        numChannels = 2;
        usesFloatingPointData = false;
    }

    bool readSamples(int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                     juce::int64 startSampleInFile, int numSamples) override
    {
        for (int ch = 0; ch < numDestChannels; ++ch)
            if (destSamples[ch] != nullptr)
                juce::FloatVectorOperations::clear((float*)destSamples[ch] + startOffsetInDestBuffer, numSamples); // Mock silence
        return true;
    }
};

int main()
{
    juce::ScopedJuceInitialiser_GUI guiInit;

    AudioPlayer player;
    // We cannot easily load a file without a real file system or mocking format manager deeply.
    // However, we can test setPositionConstrained logic directly via PlaybackHelpers?
    // Or we can mock the transport source behavior if AudioPlayer allows injection? It doesn't.

    // Test logic:
    // 1. Verify setPositionConstrained clamps correctly.
    // 2. Verify getEffectiveDuration logic in SessionState.

    SessionState state;
    state.cutModeActive = true;
    double duration = state.getEffectiveDuration(10.0, 2.0, 8.0);
    if (std::abs(duration - 6.0) > 0.001) return 1;

    state.cutModeActive = false;
    duration = state.getEffectiveDuration(10.0, 2.0, 8.0);
    if (std::abs(duration - 10.0) > 0.001) return 2;

    // Test PlaybackHelpers directly as it is static
    double pos = PlaybackHelpers::constrainPosition(1.0, 2.0, 8.0); // Should be 2.0 (clamped to cutIn)
    if (std::abs(pos - 2.0) > 0.001) return 3;

    pos = PlaybackHelpers::constrainPosition(9.0, 2.0, 8.0); // Should be 8.0 (clamped to cutOut)
    if (std::abs(pos - 8.0) > 0.001) return 4;

    pos = PlaybackHelpers::constrainPosition(5.0, 2.0, 8.0); // Should be 5.0
    if (std::abs(pos - 5.0) > 0.001) return 5;

    return 0;
}
