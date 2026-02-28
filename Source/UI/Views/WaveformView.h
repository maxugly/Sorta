#ifndef AUDIOFILER_WAVEFORMVIEW_H
#define AUDIOFILER_WAVEFORMVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Utils/Config.h"

/**
 * @file WaveformView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief View component responsible for rendering the base audio waveform.
 */

/**
 * @struct WaveformViewState
 * @brief Encapsulates the visual data required to render the base waveform.
 */
struct WaveformViewState {
    /** @brief Pointer to the audio thumbnail provider. */
    juce::AudioThumbnail* thumbnail{nullptr};
    /** @brief The total duration of the audio in seconds. */
    double totalLength{0.0};
    /** @brief The current channel view mode. */
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};
};

/**
 * @class WaveformView
 * @brief A passive UI component that renders the underlying audio waveform.
 * 
 * @details Architecturally, the WaveformView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the audio data provided by its 
 *          associated state struct. It employs an image-based caching strategy 
 *          for efficient repainting. It relies entirely on the CutPresenter 
 *          to push updates via updateState().
 * 
 * @see CutPresenter, WaveformCanvasView, ControlPanel, WaveformViewState
 */
class WaveformView : public juce::Component {
  public:
    /**
     * @brief Constructs a new WaveformView.
     */
    WaveformView();

    /** @brief Destructor. */
    ~WaveformView() override;

    /** @brief Standard JUCE paint callback, utilizing the cached waveform image. */
    void paint(juce::Graphics &g) override;

    /** 
     * @brief Updates the view's internal state and marks the cache as dirty. 
     * @param newState The new visual state to apply.
     */
    void updateState(const WaveformViewState& newState);

    /** @brief Force-clears the internal image cache and marks the view for redrawing. */
    void clearCaches();

  private:
    /** 
     * @brief Internal helper to draw the waveform data into the graphics context. 
     * @param g The graphics context to draw into.
     */
    void drawWaveform(juce::Graphics &g);

    WaveformViewState state;
    juce::Image cachedWaveform;
    bool isCacheDirty{true};
    int loadingTickCounter{0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView)
};

#endif
