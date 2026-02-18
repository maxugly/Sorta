#ifndef AUDIOFILER_WAVEFORMRENDERER_H
#define AUDIOFILER_WAVEFORMRENDERER_H

#include <JuceHeader.h>
#include "PlaybackCursorGlow.h"

class ControlPanel;
class AudioPlayer;

/**
 * @class WaveformRenderer
 * @brief Renders the waveform, loop overlays, playback cursor, and mouse feedback for the ControlPanel.
 *
 * Extracting this class keeps ControlPanel::paint concise while making it easier to reason about
 * future visual tweaks or alternate render modes.
 */
class WaveformRenderer : public juce::ChangeListener
{
public:
    void invalidateWaveformCache();
    /**
     * @brief Constructs a renderer bound to a ControlPanel.
     * @param controlPanel Reference to the owning ControlPanel for accessing state and helpers.
     */
    explicit WaveformRenderer(ControlPanel& controlPanel);
    ~WaveformRenderer() override;

    /**
     * @brief Paints the waveform and overlays for the ControlPanel.
     * @param g Graphics context supplied by ControlPanel::paint.
     */
    /**
     * @brief Paints the cached waveform (static).
     * @param g Graphics context supplied by ControlPanel::paint.
     */
    void renderWaveform(juce::Graphics& g);

    /**
     * @brief Paints the dynamic overlays (cursors, zoom, etc.).
     * @param g Graphics context supplied by PlaybackOverlay::paint.
     */
    void renderOverlays(juce::Graphics& g);

private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    mutable juce::Image waveformCache;
    mutable juce::Rectangle<int> lastBounds;
    mutable double lastAudioLength = -1.0;
    mutable float lastScale = 1.0f;
    mutable int lastQuality = -1;
    mutable int lastChannelMode = -1;
    void drawWaveform(juce::Graphics& g, AudioPlayer& audioPlayer) const;
    void drawReducedQualityWaveform(juce::Graphics& g, AudioPlayer& audioPlayer, int channel, int pixelsPerSample) const;
    void drawCutModeOverlays(juce::Graphics& g, AudioPlayer& audioPlayer, float audioLength) const;
    void drawPlaybackCursor(juce::Graphics& g, AudioPlayer& audioPlayer, float audioLength) const;
    void drawMouseCursorOverlays(juce::Graphics& g, AudioPlayer& audioPlayer, float audioLength) const;
    void drawZoomPopup(juce::Graphics& g) const;

    ControlPanel& controlPanel;
};

#endif // AUDIOFILER_WAVEFORMRENDERER_H
