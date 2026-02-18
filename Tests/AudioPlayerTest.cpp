#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../Source/AudioPlayer.h"
#include "../Source/SessionState.h"

// Simple Mock Source
class MockAudioSource : public juce::PositionableAudioSource
{
public:
    MockAudioSource() { lengthInSamples = 44100 * 60; } // 1 minute

    void setNextReadPosition (juce::int64 newPosition) override { position = newPosition; }
    juce::int64 getNextReadPosition() const override { return position; }
    juce::int64 getTotalLength() const override { return lengthInSamples; }
    bool isLooping() const override { return false; }
    void setLooping (bool) override {}

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
    }
    void releaseResources() override {}
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override {}

    double currentSampleRate = 44100.0;
    juce::int64 lengthInSamples = 0;
    juce::int64 position = 0;
};

class AudioPlayerTest : public juce::UnitTest
{
public:
    AudioPlayerTest() : juce::UnitTest("AudioPlayer Testing") {}

    void runTest() override
    {
        beginTest("setPlayheadPosition clamps position correctly");
        {
            // Create AudioPlayer
            SessionState sessionState;
            AudioPlayer player(sessionState);

            // Setup Mock Source
            MockAudioSource mockSource;

            // We need to initialize transport source with a source
            player.getTransportSource().setSource(&mockSource, 0, nullptr, 44100.0);
            player.getTransportSource().prepareToPlay(512, 44100.0);

            // Verify initial state
            expectEquals(player.getTransportSource().getCurrentPosition(), 0.0);

            // Mock cut points
            player.setCutIn(2.0);
            player.setCutOut(8.0);

            // Test case 1: Position within range
            player.setPlayheadPosition(5.0);
            expectEquals(player.getTransportSource().getCurrentPosition(), 5.0);

            // Test case 2: Position outside range (below)
            player.setPlayheadPosition(1.0);
            expectEquals(player.getTransportSource().getCurrentPosition(), 2.0);

            // Test case 3: Position outside range (above)
            player.setPlayheadPosition(9.0);
            expectEquals(player.getTransportSource().getCurrentPosition(), 8.0);

            // Test case 4: Swapped cut points
            player.setCutIn(8.0);
            player.setCutOut(2.0);
            player.setPlayheadPosition(5.0);
            expectEquals(player.getTransportSource().getCurrentPosition(), 5.0);

            player.setPlayheadPosition(1.0);
            expectEquals(player.getTransportSource().getCurrentPosition(), 2.0); // Expect min(cutIn, cutOut) which is 2.0

            // Cleanup
            player.getTransportSource().setSource(nullptr);
        }
    }
};

static AudioPlayerTest audioPlayerTest;
