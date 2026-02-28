/**
 * @file AudioPlayerTest.cpp
 * @Source/Core/FileMetadata.h
 * @ingroup Tests
 * @brief Verifies the audio engine's playback logic, playhead clamping, and boundary synchronization.
 */

#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

/**
 * @class MockAudioSource
 * @brief A lightweight mock audio source for testing the transport engine in a headless context.
 * 
 * @details This mock implements `juce::PositionableAudioSource` and provides 
 *          predictable playback metadata, allowing the `AudioPlayer` to be tested 
 *          mathematically without actual disk I/O or an active audio output device.
 */
class MockAudioSource : public juce::PositionableAudioSource {
  public:
    MockAudioSource() {
        lengthInSamples = 44100 * 60;
    } // 1 minute

    void setNextReadPosition(juce::int64 newPosition) override {
        position = newPosition;
    }
    juce::int64 getNextReadPosition() const override {
        return position;
    }
    juce::int64 getTotalLength() const override {
        return lengthInSamples;
    }
    bool isLooping() const override {
        return false;
    }
    void setLooping(bool) override {
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        currentSampleRate = sampleRate;
    }
    void releaseResources() override {
    }
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        juce::ignoreUnused(bufferToFill);
    }

    double currentSampleRate = 44100.0;
    juce::int64 lengthInSamples = 0;
    juce::int64 position = 0;
};

/**
 * @class AudioPlayerTest
 * @brief Unit test suite for verifying the core playback engine.
 * 
 * @details Architecturally, this test suite validates the "Headless Portability" law, 
 *          proving that the engine and core logic (playback, seeking, clamping) function 
 *          perfectly without any GUI components or visual waveforms attached.
 * 
 *          These tests verify the strict "Separation of Concerns" (SoC) law, 
 *          ensuring `AudioPlayer` strictly handles file reading and playback state 
 *          without bleeding into UI logic. It confirms that calculations for playhead 
 *          positions and cut-boundary synchronization are mathematically deterministic 
 *          at the engine level.
 */
class AudioPlayerTest : public juce::UnitTest {
  public:
    AudioPlayerTest() : juce::UnitTest("AudioPlayer Testing") {
    }

    /** 
     * @brief Executes playback logic verification cases.
     * @details Focuses on validating that the engine correctly clamps the 
     *          playhead position within the user-defined 'In' and 'Out' boundaries 
     *          in all possible scenarios (within range, below range, above range, swapped points).
     */
    void runTest() override {
        beginTest("setPlayheadPosition clamps position correctly");
        {
            // Create AudioPlayer
            SessionState sessionState;
            AudioPlayer player(sessionState);

            // Setup Mock Source
            MockAudioSource mockSource;

            // We need to initialize transport source with a source
            player.setSourceForTesting(&mockSource, 44100.0);
            player.prepareToPlay(512, 44100.0);

            // Verify initial state
            expectEquals(player.getCurrentPosition(), 0.0);

            // Mock cut points
            sessionState.setCutActive(true);
            player.setCutIn(2.0);
            player.setCutOut(8.0);

            // Test case 1: Position within range
            player.setPlayheadPosition(5.0);
            expectEquals(player.getCurrentPosition(), 5.0);

            // Test case 2: Position outside range (below)
            player.setPlayheadPosition(1.0);
            expectEquals(player.getCurrentPosition(), 2.0);

            // Test case 3: Position outside range (above)
            player.setPlayheadPosition(9.0);
            expectEquals(player.getCurrentPosition(), 8.0);

            // Test case 4: Swapped cut points
            player.setCutIn(8.0);
            player.setCutOut(2.0);
            player.setPlayheadPosition(5.0);
            expectEquals(player.getCurrentPosition(), 5.0);

            player.setPlayheadPosition(1.0);
            expectEquals(player.getCurrentPosition(),
                         2.0); // Expect min(cutIn, cutOut) which is 2.0

            // Cleanup
            player.setSourceForTesting(nullptr, 0.0);
        }
    }
};

static AudioPlayerTest audioPlayerTest;
