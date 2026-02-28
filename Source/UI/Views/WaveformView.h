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

class WaveformManager;

/**
 * @class WaveformView
 * @brief A passive UI component that renders the underlying audio waveform.
 * 
 * @details Architecturally, the WaveformView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the audio data provided by the 
 *          WaveformManager (the data model for visual waveform data). It 
 *          employs an image-based caching strategy to ensure efficient 
 *          repainting during UI interactions. It relies entirely on external 
 *          triggers (via WaveformManager changes) to invalidate its cache 
 *          and redraw.
 * 
 * @see WaveformManager, WaveformCanvasView, ControlPanel
 */
class WaveformView : public juce::Component, public juce::ChangeListener {
  public:
    /**
     * @brief Constructs a new WaveformView.
     * @param waveformManager Reference to the data provider for waveform rendering.
     */
    explicit WaveformView(WaveformManager &waveformManager);

    /** @brief Destructor. */
    ~WaveformView() override;

    /** 
     * @brief Sets whether the waveform should display mono or stereo channels. 
     * @param channelMode The new channel view mode.
     */
    void setChannelMode(AppEnums::ChannelViewMode channelMode);

    /** @brief Standard JUCE paint callback, utilizing the cached waveform image. */
    void paint(juce::Graphics &g) override;

    /** 
     * @brief Standard JUCE change listener callback. 
     * @param source The broadcaster that triggered the change, typically WaveformManager.
     */
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    /** @brief Force-clears the internal image cache and marks the view for redrawing. */
    void clearCaches();

  private:
    /** 
     * @brief Internal helper to draw the waveform data into the graphics context. 
     * @param g The graphics context to draw into.
     */
    void drawWaveform(juce::Graphics &g);

    WaveformManager &waveformManager;
    AppEnums::ChannelViewMode currentChannelMode = AppEnums::ChannelViewMode::Mono;

    juce::Image cachedWaveform;
    bool isCacheDirty{true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView)
};

#endif
