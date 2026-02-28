#ifndef AUDIOFILER_WAVEFORMMANAGER_H
#define AUDIOFILER_WAVEFORMMANAGER_H

#if defined(JUCE_HEADLESS)
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file WaveformManager.h
 * @ingroup Logic
 * @brief Logic-tier manager for waveform visualization and thumbnail caching.
 * 
 * @details Architecturally, WaveformManager serves as a "Resource Manager" that 
 *          bridges the gap between raw audio files and the visual display layer. 
 *          It encapsulates JUCE's `juce::AudioThumbnail` and `juce::AudioThumbnailCache` 
 *          to provide efficient, multi-level waveform rendering.
 * 
 *          Key responsibilities:
 *          - **Asynchronous Analysis**: Automatically generates low-resolution 
 *            waveform data in the background upon file loading.
 *          - **Cache Management**: Maintains a pool of recently analyzed waveforms 
 *            to ensure instant navigation between previously opened assets.
 *          - **Change Broadcasting**: Notifies waveform views (via standard 
 *            `juce::ChangeListener`) when thumbnail data becomes available or 
 *            changes due to background analysis.
 * 
 * @see AudioPlayer
 * @see WaveformView
 * @see AudioThumbnail
 */
class WaveformManager {
  public:
    /**
     * @brief Constructs the manager and initializes the thumbnail engine.
     * @param formatManagerIn Reference to the application-wide format decoder.
     */
    explicit WaveformManager(juce::AudioFormatManager &formatManagerIn);

    /**
     * @brief Initiates waveform analysis for a new file.
     * @param file The audio asset to analyze.
     * @details This triggers a background process that updates the thumbnail cache. 
     *          The view will be notified of progress via change listeners.
     */
    void loadFile(const juce::File &file);

    /**
     * @brief Provides access to the primary thumbnail object.
     * @return Reference to the internal AudioThumbnail.
     */
    juce::AudioThumbnail &getThumbnail();

    /**
     * @brief Provides read-only access to the primary thumbnail object.
     * @return Const reference to the AudioThumbnail.
     */
    const juce::AudioThumbnail &getThumbnail() const;

    /**
     * @brief Registers a view or component to receive thumbnail update notifications.
     * @param listener The observer implementing juce::ChangeListener.
     */
    void addChangeListener(juce::ChangeListener *listener);

    /**
     * @brief Unregisters an observer.
     * @param listener The observer to remove.
     */
    void removeChangeListener(juce::ChangeListener *listener);

  private:
    juce::AudioFormatManager &formatManager;          /**< Dependency for audio decoding. */
    juce::AudioThumbnailCache thumbnailCache{5};      /**< Memory-backed cache for 5 concurrent thumbnails. */
    juce::AudioThumbnail thumbnail{512, formatManager, thumbnailCache}; /**< The primary waveform data source. */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformManager)
};

#endif
