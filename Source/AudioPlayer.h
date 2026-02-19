

#ifndef AUDIOFILER_AUDIOPLAYER_H
#define AUDIOFILER_AUDIOPLAYER_H

#if defined(JUCE_HEADLESS)
    #include <juce_core/juce_core.h>
    #include <juce_audio_basics/juce_audio_basics.h>
    #include <juce_audio_formats/juce_audio_formats.h>
    #include <juce_audio_devices/juce_audio_devices.h>
    #include <juce_events/juce_events.h>
#else
    #include <JuceHeader.h>
#endif

#include "Config.h"
#include "SessionState.h"
#include "MainDomain.h"
#if !defined(JUCE_HEADLESS)
#include "WaveformManager.h"
#endif
#include <mutex>

class ControlPanel;

/**
 * @ingroup Audio
 * @class AudioPlayer
 * @brief The main engine for real-time playback and file buffering.
 * @details This class manages the audio device callback, file loading via
 * `juce::AudioTransportSource`, and background buffering. It synchronizes
 * playback state with `SessionState` and updates the UI via `ControlPanel`.
 * @see SessionState
 * @see ControlPanel
 */
class AudioPlayer : public juce::AudioSource,
                    public juce::ChangeListener,
                    public juce::ChangeBroadcaster,
                    public SessionState::Listener
{
public:

    explicit AudioPlayer(SessionState& state);

    ~AudioPlayer() override;

    void setPlayheadPosition(double seconds);

    juce::Result loadFile(const juce::File& file);

    void togglePlayStop();

    bool isPlaying() const;

    double getCurrentPosition() const;

    void setControlPanel(ControlPanel* panel) { controlPanel = panel; }

    void startSilenceAnalysis(float threshold, bool detectingIn);

    bool isRepeating() const;

    void setRepeating(bool shouldRepeat);

    #if !defined(JUCE_HEADLESS)

    juce::AudioThumbnail& getThumbnail();

    WaveformManager& getWaveformManager();

    const WaveformManager& getWaveformManager() const;
    #endif

    void startPlayback();

    void stopPlayback();

    juce::AudioFormatManager& getFormatManager();

    juce::AudioFormatReader* getAudioFormatReader() const;

    juce::File getLoadedFile() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /**
     * @brief Processes the next block of audio samples.
     * @details Fetches audio from the `transportSource`, applies gain/processing,
     * and writes to the output buffer. This runs on the high-priority audio thread.
     * @param bufferToFill The buffer structure to populate with audio data.
     */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    void releaseResources() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) override;

    double getCutIn() const { return sessionState.getCutIn(); }

    double getCutOut() const { return sessionState.getCutOut(); }

    void setCutIn(double positionSeconds) { sessionState.setCutIn(positionSeconds); }

    void setCutOut(double positionSeconds) { sessionState.setCutOut(positionSeconds); }

    std::mutex& getReaderMutex() { return readerMutex; }

    bool getReaderInfo(double& sampleRateOut, juce::int64& lengthInSamplesOut) const;

#if JUCE_UNIT_TESTS

    void setSourceForTesting(juce::PositionableAudioSource* source, double sampleRate);
#endif

private:
    /** @brief Handles decoding of supported audio file formats (WAV, MP3, etc.). */
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    /** @brief Background thread used by the transport source to buffer audio data. */
    juce::TimeSliceThread readAheadThread;
    /** @brief High-level source that handles resampling, buffering, and playback control. */
    juce::AudioTransportSource transportSource;

    #if !defined(JUCE_HEADLESS)
    WaveformManager waveformManager;
    #endif

    juce::File loadedFile;
    /** @brief Reference to the shared application state. */
    SessionState& sessionState;
    ControlPanel* controlPanel{nullptr};
    float lastAutoCutThresholdIn{-1.0f};
    float lastAutoCutThresholdOut{-1.0f};
    bool lastAutoCutInActive{false};
    bool lastAutoCutOutActive{false};
    mutable std::mutex readerMutex;

    bool repeating = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayer)
};

#endif 
