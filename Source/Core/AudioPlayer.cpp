/**
 * @file AudioPlayer.cpp
 */
#include "Core/AudioPlayer.h"
#include "Core/FileMetadata.h"
#include "Core/SessionState.h"
#include "Utils/PlaybackHelpers.h"
#include <algorithm>
#include <cmath>

AudioPlayer::AudioPlayer(SessionState &state)
#if !defined(JUCE_HEADLESS)
    : waveformManager(formatManager),
#else
    :
#endif
      readAheadThread(Config::Labels::threadAudioReader), sessionState(state) {
    formatManager.registerBasicFormats();
    sessionState.addListener(this);
    readAheadThread.startThread();
    transportSource.addChangeListener(this);

    lastAutoCutThresholdIn = sessionState.getCutPrefs().autoCut.thresholdIn;
    lastAutoCutThresholdOut = sessionState.getCutPrefs().autoCut.thresholdOut;
    lastAutoCutInActive = sessionState.getCutPrefs().autoCut.inActive;
    lastAutoCutOutActive = sessionState.getCutPrefs().autoCut.outActive;

    cachedCutActive = sessionState.getCutPrefs().active;
    cachedCutIn = sessionState.getCutIn();
    cachedCutOut = sessionState.getCutOut();
}

AudioPlayer::~AudioPlayer() {
    sessionState.removeListener(this);
    transportSource.setSource(nullptr);
    readAheadThread.stopThread(1000);
    transportSource.removeChangeListener(this);
}

juce::Result AudioPlayer::loadFile(const juce::File &file) {
    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        const juce::String filePath = file.getFullPathName();
        const double totalDuration = (double)reader->lengthInSamples / reader->sampleRate;
        sessionState.setTotalDuration(totalDuration);

        if (sessionState.hasMetadataForFile(filePath)) {
            const FileMetadata cached = sessionState.getMetadataForFile(filePath);
            sessionState.setMetadataForFile(filePath, cached);
        } else {
            FileMetadata metadata;
            if (reader->sampleRate > 0.0)
                metadata.cutOut = totalDuration;
            sessionState.setMetadataForFile(filePath, metadata);
        }

        lastAutoCutThresholdIn = sessionState.getCutPrefs().autoCut.thresholdIn;
        lastAutoCutThresholdOut = sessionState.getCutPrefs().autoCut.thresholdOut;
        lastAutoCutInActive = sessionState.getCutPrefs().autoCut.inActive;
        lastAutoCutOutActive = sessionState.getCutPrefs().autoCut.outActive;

        loadedFile = file;
        {
            std::lock_guard<std::mutex> lock(readerMutex);
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource.setSource(newSource.get(), Config::Audio::readAheadBufferSize,
                                      &readAheadThread, reader->sampleRate);
#if !defined(JUCE_HEADLESS)
            waveformManager.loadFile(file);
#endif
            cachedSampleRate = reader->sampleRate;
            cachedTotalSamples = reader->lengthInSamples;
            readerSource.reset(newSource.release());
        }
        transportSource.setGain(sessionState.getVolume());
        setPlayheadPosition(sessionState.getCutPrefs().cutIn);

        sessionState.setCurrentFilePath(filePath);

        return juce::Result::ok();
    }

    return juce::Result::fail(Config::Labels::failGeneric);
}

juce::File AudioPlayer::getLoadedFile() const {
    return loadedFile;
}

void AudioPlayer::togglePlayStop() {
    if (transportSource.isPlaying())
        transportSource.stop();
    else
        transportSource.start();
}

bool AudioPlayer::isPlaying() const {
    return transportSource.isPlaying();
}

double AudioPlayer::getCurrentPosition() const {
    return transportSource.getCurrentPosition();
}

bool AudioPlayer::isRepeating() const {
    return repeating;
}

void AudioPlayer::setRepeating(bool shouldRepeat) {
    repeating = shouldRepeat;
}

#if !defined(JUCE_HEADLESS)
juce::AudioThumbnail &AudioPlayer::getThumbnail() {
    return waveformManager.getThumbnail();
}

WaveformManager &AudioPlayer::getWaveformManager() {
    return waveformManager;
}

const WaveformManager &AudioPlayer::getWaveformManager() const {
    return waveformManager;
}
#endif

void AudioPlayer::startPlayback() {
    transportSource.start();
}

void AudioPlayer::stopPlayback() {
    transportSource.stop();
}

void AudioPlayer::stopPlaybackAndReset() {
    transportSource.stop();
    setPlayheadPosition(sessionState.getCutIn());
}

juce::AudioFormatManager &AudioPlayer::getFormatManager() {
    return formatManager;
}

void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/**
 * @details This method is the performance-critical heart of the audio engine. It operates 
 *          under strict "Lock-Free Deterministic" constraints to ensure jitter-free 
 *          audio processing.
 *          
 *          Key Algorithmic Pillars:
 *          1. **Atomic State Synchronization**: Instead of locking a mutex (which 
 *             risks priority inversion), it cross-references `cachedCutIn` and 
 *             `cachedCutOut`—atomic variables updated from the Message Thread. 
 *             This ensures that UI-driven boundary changes are applied at the 
 *             very next block boundary.
 *          2. **Boundary Enforcement**: If the current `startPos` is already past 
 *             the `cutOut` point, the method immediately stops playback or wraps 
 *             to the `cutIn` point (if `repeating` is active).
 *          3. **Buffer Truncation & Zero-Leakage**: If the block currently being processed 
 *             spans across the `cutOut` point, we mathematically calculate the exact 
 *             sample index (`samplesToKeep`) where the cut occurs. We then 
 *             forcefully clear (zero out) the remainder of the buffer. This 
 *             physically prevents audio data leakage past a user-defined cut marker, 
 *             guaranteeing that exported files or live previews never "overshoot" 
 *             the silence threshold.
 *          4. **Micro-Fades (Implicit)**: By stopping at the exact sample boundary, 
 *             we rely on the underlying transport's gain-ramping (if applicable) 
 *             to prevent pops, while maintaining mathematical frame-accuracy.
 */
void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (readerSource.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if (!cachedCutActive) {
        transportSource.getNextAudioBlock(bufferToFill);
        return;
    }

    const double sampleRate = cachedSampleRate;
    const juce::int64 lengthInSamples = cachedTotalSamples;

    if (sampleRate <= 0.0) {
        transportSource.getNextAudioBlock(bufferToFill);
        return;
    }

    const double cutIn = cachedCutIn;
    const double cutOut = std::max(cutIn, (double)cachedCutOut);
    const double startPos = transportSource.getCurrentPosition();

    // Catch-all: If the playhead somehow jumped past the cut point, reset or stop now.
    if (startPos >= cutOut) {
        if (repeating) {
            transportSource.setPosition(cutIn);
            transportSource.start();
            transportSource.getNextAudioBlock(bufferToFill);
        } else {
            transportSource.stop();
            transportSource.setPosition(cutOut);
            bufferToFill.clearActiveBufferRegion();
        }
        return;
    }

    // Fill the buffer with raw audio data first
    transportSource.getNextAudioBlock(bufferToFill);

    // Calculate the time-position of the final sample in this block
    const double endPos = startPos + ((double)bufferToFill.numSamples / sampleRate);
    
    // Check if the current block straddles the 'Out' boundary
    if (endPos >= cutOut) {
        // Calculate exactly how many samples should remain before silence begins
        const int samplesToKeep = juce::jlimit(0, bufferToFill.numSamples,
                                               (int)std::floor((cutOut - startPos) * sampleRate));

        // Zero-out the "leakage" samples to enforce the hard cut
        if (samplesToKeep < bufferToFill.numSamples) {
            bufferToFill.buffer->clear(bufferToFill.startSample + samplesToKeep,
                                       bufferToFill.numSamples - samplesToKeep);
        }

        // Loop back or halt the transport as necessary
        if (repeating) {
            transportSource.setPosition(cutIn);
            transportSource.start();
        } else {
            transportSource.stop();
            transportSource.setPosition(cutOut);
        }
    }
}

void AudioPlayer::releaseResources() {
    transportSource.releaseResources();
}

void AudioPlayer::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &transportSource) {
        sendChangeMessage();
    }
}

void AudioPlayer::cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
    const auto &autoCut = prefs.autoCut;

    lastAutoCutThresholdIn = autoCut.thresholdIn;
    lastAutoCutThresholdOut = autoCut.thresholdOut;
    lastAutoCutInActive = autoCut.inActive;
    lastAutoCutOutActive = autoCut.outActive;

    cachedCutActive = prefs.active;
    cachedCutIn = prefs.cutIn;
    cachedCutOut = prefs.cutOut;
}

void AudioPlayer::cutInChanged(double value) {
    cachedCutIn = value;
}

void AudioPlayer::cutOutChanged(double value) {
    cachedCutOut = value;
}

void AudioPlayer::volumeChanged(float newVolume) {
    transportSource.setGain(newVolume);
}

juce::AudioFormatReader *AudioPlayer::getAudioFormatReader() const {
    if (readerSource != nullptr)
        return readerSource->getAudioFormatReader();
    return nullptr;
}

bool AudioPlayer::getReaderInfo(double &sampleRateOut, juce::int64 &lengthInSamplesOut) const {
    std::lock_guard<std::mutex> lock(readerMutex);
    if (readerSource == nullptr)
        return false;

    auto *reader = readerSource->getAudioFormatReader();
    if (reader == nullptr)
        return false;

    sampleRateOut = reader->sampleRate;
    lengthInSamplesOut = reader->lengthInSamples;
    return true;
}

#if JUCE_UNIT_TESTS
void AudioPlayer::setSourceForTesting(juce::PositionableAudioSource *source, double sampleRate) {
    transportSource.setSource(source, 0, nullptr, sampleRate);
}
#endif

void AudioPlayer::setPlayheadPosition(double seconds) {
    const double sampleRate = cachedSampleRate;
    const juce::int64 lengthInSamples = cachedTotalSamples;

    if (sampleRate <= 0.0)
        return;

    const double totalDuration = (double)lengthInSamples / sampleRate;

    double cutIn = 0.0;
    double cutOut = totalDuration;

    if (cachedCutActive) {
        cutIn = std::min((double)cachedCutIn, totalDuration);
        cutOut = std::clamp((double)cachedCutOut, cutIn, totalDuration);
    }

    double clampedPos = juce::jlimit(cutIn, cutOut, seconds);
    transportSource.setPosition(clampedPos);
}
