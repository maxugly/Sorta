#ifndef AUDIOFILER_AUDIOPLAYER_H
#define AUDIOFILER_AUDIOPLAYER_H

#if defined(JUCE_HEADLESS)
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"
#include "MainDomain.h"
#include "Utils/Config.h"
#if !defined(JUCE_HEADLESS)
#include "Core/WaveformManager.h"
#endif
#include <mutex>
#include <atomic>

/**
 * @file AudioPlayer.h
 * @ingroup AudioEngine
 * @brief The primary playback engine and audio asset manager.
 * 
 * @details Colloquially known as the "Audio Engine," this class is responsible for the 
 *          entire lifecycle of audio playback within the application. It acts as a 
 *          high-level wrapper around JUCE's `juce::AudioTransportSource` and 
 *          `juce::AudioFormatReaderSource`.
 * 
 *          Architecturally, the AudioPlayer is a "Fat Model" that manages:
 *          - **File I/O**: Thread-safe loading and unloading of audio files.
 *          - **Buffering**: Utilizes a private `juce::TimeSliceThread` for background 
 *            read-ahead buffering, ensuring glitch-free playback even with high-latency 
 *            storage devices.
 *          - **Real-time Processing**: Implements `juce::AudioSource` to provide the 
 *            sample stream to the hardware device, enforcing cut boundaries in real-time.
 *          - **State Synchronization**: Observes `SessionState` to react to user 
 *            adjustments (volume, boundaries, locks) without UI thread intervention.
 * 
 *          The AudioPlayer maintains an internal "air gap" via mutexes and atomics 
 *          between the high-priority Audio Thread and the lower-priority Message Thread.
 * 
 * @see SessionState
 * @see MainComponent
 * @see WaveformManager
 * @see AudioSource
 */
class AudioPlayer : public juce::AudioSource,
                    public juce::ChangeListener,
                    public juce::ChangeBroadcaster,
                    public SessionState::Listener {
  public:
    /**
     * @brief Constructs the AudioPlayer and wires it to the global application state.
     * @param state Reference to the shared SessionState (The Brain).
     */
    explicit AudioPlayer(SessionState &state);

    /**
     * @brief Destructor ensures clean termination of background threads and audio sources.
     */
    ~AudioPlayer() override;

    /** 
     * @brief Seeks to the specified position in seconds, clamped by cut boundaries if active. 
     * @param seconds The target position in seconds from the start of the file.
     * @details If 'Cut Mode' is active in SessionState, the seek target is clamped 
     *          between the current 'In' and 'Out' markers.
     */
    void setPlayheadPosition(double seconds);

    /** 
     * @brief Loads an audio file and synchronizes SessionState with its metadata. 
     * @param file The juce::File handle to the audio asset.
     * @return juce::Result indicating success or failure (e.g., unsupported format).
     * @note This operation is thread-safe and involves a mutex-protected reader swap.
     */
    juce::Result loadFile(const juce::File &file);

    /** 
     * @brief Toggles between playback and paused states. 
     * @details If currently playing, it stops. If stopped, it starts from the current 
     *          position (or the 'In' point if at/past the 'Out' point).
     */
    void togglePlayStop();

    /** 
     * @brief Returns true if the transport is currently playing. 
     * @return Boolean state of the underlying transport source.
     */
    bool isPlaying() const;

    /** 
     * @brief Returns the current transport position in seconds. 
     * @return Double precision time in seconds.
     */
    double getCurrentPosition() const;

    /** 
     * @brief Returns true if the player is set to repeat between cut points. 
     * @return Boolean repeat status.
     */
    bool isRepeating() const;

    /** 
     * @brief Sets whether the player should repeat between cut points. 
     * @param shouldRepeat True to loop the playback region.
     */
    void setRepeating(bool shouldRepeat);

#if !defined(JUCE_HEADLESS)

    /** 
     * @brief Returns the audio thumbnail for waveform rendering. 
     * @return Reference to the internal AudioThumbnail.
     */
    juce::AudioThumbnail &getThumbnail();

    /** 
     * @brief Provides access to the WaveformManager for thumbnail updates. 
     * @return Reference to the internal WaveformManager.
     */
    WaveformManager &getWaveformManager();

    /** 
     * @brief Provides read-only access to the WaveformManager. 
     * @return Const reference to the WaveformManager.
     */
    const WaveformManager &getWaveformManager() const;
#endif

    /** @brief Starts audio playback. */
    void startPlayback();

    /** @brief Stops audio playback. */
    void stopPlayback();

    /** 
     * @brief Stops playback and seeks back to the cut-in position. 
     * @details Primarily used by 'Reset' presenters to prepare for a fresh audition.
     */
    void stopPlaybackAndReset();

    /** 
     * @brief Provides access to the global audio format manager. 
     * @return Reference to the internal juce::AudioFormatManager.
     */
    juce::AudioFormatManager &getFormatManager();

    /** 
     * @brief Returns the underlying audio format reader for the loaded file. 
     * @return Pointer to the current reader, or nullptr if no file is loaded.
     */
    juce::AudioFormatReader *getAudioFormatReader() const;

    /** 
     * @brief Returns the juce::File handle for the currently loaded audio. 
     * @return File object.
     */
    juce::File getLoadedFile() const;

    /** 
     * @brief Initializes audio processing parameters. 
     * @param samplesPerBlockExpected The number of samples the host will request per block.
     * @param sampleRate The hardware sample rate in Hz.
     */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /**
     * @brief Processes the next block of audio samples.
     * @details This is the core audio processing callback, executed on the high-priority 
     *          Audio Thread. It must be lock-free and deterministic.
     * 
     *          The logic sequence is:
     *          1. Check if a valid reader source exists. If not, clear the buffer.
     *          2. Retrieve the current cut preferences from `SessionState` via atomics.
     *          3. If cut mode is inactive, simply delegate to `transportSource`.
     *          4. If active, check the current playback position against `cutIn` and `cutOut`.
     *          5. If the position exceeds `cutOut`:
     *             - If repeating is enabled, seek back to `cutIn` immediately.
     *             - If not, stop playback and notify listeners.
     *          6. If the current block crosses the `cutOut` boundary, the player 
     *             truncates the buffer to prevent audio leakage past the marker.
     *
     * @param bufferToFill The buffer structure to populate with audio data.
     * @warning Do NOT perform any I/O, memory allocation, or UI updates here.
     */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    /** 
     * @brief Releases resources when playback stops or the device is closed. 
     */
    void releaseResources() override;

    /** 
     * @brief Handles change notifications from the underlying transport source. 
     */
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    /** 
     * @brief Reacts to global preference changes in SessionState. 
     * @param prefs The updated CutPreferences.
     */
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

    /** 
     * @brief Reacts to 'In' boundary changes. 
     * @param value New position in seconds.
     */
    void cutInChanged(double value) override;

    /** 
     * @brief Reacts to 'Out' boundary changes. 
     * @param value New position in seconds.
     */
    void cutOutChanged(double value) override;

    /** 
     * @brief Reacts to master volume adjustments. 
     * @param newVolume New gain level.
     */
    void volumeChanged(float newVolume) override;

    /** @brief Gets the current 'In' marker from state. */
    double getCutIn() const {
        return sessionState.getCutIn();
    }

    /** @brief Gets the current 'Out' marker from state. */
    double getCutOut() const {
        return sessionState.getCutOut();
    }

    /** @brief Updates the 'In' marker in state. */
    void setCutIn(double positionSeconds) {
        sessionState.setCutIn(positionSeconds);
    }

    /** @brief Updates the 'Out' marker in state. */
    void setCutOut(double positionSeconds) {
        sessionState.setCutOut(positionSeconds);
    }

    /**
     * @brief Retrieves technical info about the currently loaded asset.
     * @param sampleRateOut Populated with the file's sample rate.
     * @param lengthInSamplesOut Populated with the file's sample length.
     * @return True if a valid reader was available.
     */
    bool getReaderInfo(double &sampleRateOut, juce::int64 &lengthInSamplesOut) const;

#if JUCE_UNIT_TESTS
    /** @brief Injects a mock source for automated testing. */
    void setSourceForTesting(juce::PositionableAudioSource *source, double sampleRate);
#endif

  private:
    juce::AudioFormatManager formatManager;              /**< Manages decoding for WAV, AIFF, MP3, etc. */
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource; /**< Direct stream from the file on disk. */
    juce::TimeSliceThread readAheadThread;               /**< Background thread for disk I/O pre-buffering. */
    juce::AudioTransportSource transportSource;          /**< JUCE transport for seek/play/pause control. */

#if !defined(JUCE_HEADLESS)
    WaveformManager waveformManager;                     /**< Manages waveform generation and display cache. */
#endif

    juce::File loadedFile;                               /**< The currently active file handle. */
    SessionState &sessionState;                          /**< Reference to the central state hub. */
    float lastAutoCutThresholdIn{-1.0f};                 /**< Cache for detecting threshold changes. */
    float lastAutoCutThresholdOut{-1.0f};                /**< Cache for detecting threshold changes. */
    bool lastAutoCutInActive{false};                    /**< Cache for auto-cut state. */
    bool lastAutoCutOutActive{false};                   /**< Cache for auto-cut state. */
    mutable std::mutex readerMutex;                      /**< Protects the readerSource during file swaps. */

    std::atomic<bool> cachedCutActive{false};           /**< Fast-access atomic for the audio thread. */
    std::atomic<double> cachedCutIn{0.0};               /**< Fast-access atomic for the audio thread. */
    std::atomic<double> cachedCutOut{0.0};              /**< Fast-access atomic for the audio thread. */
    std::atomic<double> cachedSampleRate{0.0};          /**< Fast-access atomic for the audio thread. */
    std::atomic<juce::int64> cachedTotalSamples{0};    /**< Fast-access atomic for the audio thread. */

    bool repeating = false;                              /**< Local toggle for loop playback. */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayer)
};

#endif
