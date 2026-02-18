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
#include "SilenceWorkerClient.h"
#include "SilenceAnalysisWorker.h"
#if !defined(JUCE_HEADLESS)
#include "WaveformManager.h"
#endif
#include <mutex>

/**
 * @class AudioPlayer
 * @brief Manages audio file loading, playback, and transport.
 */
class AudioPlayer : public juce::AudioSource,
                    public juce::ChangeListener,
                    public juce::ChangeBroadcaster,
                    public SessionState::Listener,
                    public SilenceWorkerClient
{
public:
    explicit AudioPlayer(SessionState& state);
    ~AudioPlayer() override;

    /** @brief Sets the playback position. */
    void setPlayheadPosition(double seconds);

    juce::Result loadFile(const juce::File& file);
    void togglePlayStop();
    bool isPlaying() const;

    /** @brief Starts a background silence analysis. */
    void startSilenceAnalysis(float threshold, bool detectingIn);

    /** @brief Checks if repeating is enabled. */
    bool isRepeating() const;

    /** @brief Enables or disables repeating for playback. */
    void setRepeating(bool shouldRepeat);

    #if !defined(JUCE_HEADLESS)
    juce::AudioThumbnail& getThumbnail();
    WaveformManager& getWaveformManager();
    #endif

    juce::AudioTransportSource& getTransportSource();
    juce::AudioFormatManager& getFormatManager();
    juce::AudioFormatReader* getAudioFormatReader() const;
    juce::File getLoadedFile() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
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

    // SilenceWorkerClient implementation
    AudioPlayer& getAudioPlayer() override { return *this; }
    void setCutStart(int sampleIndex) override;
    void setCutEnd(int sampleIndex) override;
    void logStatusMessage(const juce::String& message, bool isError = false) override;
    bool isCutModeActive() const override { return sessionState.getCutPrefs().active; }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::TimeSliceThread readAheadThread;
    juce::AudioTransportSource transportSource;

    #if !defined(JUCE_HEADLESS)
    WaveformManager waveformManager;
    #endif

    juce::File loadedFile;
    SessionState& sessionState;
    float lastAutoCutThresholdIn{-1.0f};
    float lastAutoCutThresholdOut{-1.0f};
    bool lastAutoCutInActive{false};
    bool lastAutoCutOutActive{false};
    mutable std::mutex readerMutex;

    SilenceAnalysisWorker silenceWorker;
    bool repeating = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayer)
};

#endif // AUDIOFILER_AUDIOPLAYER_H
