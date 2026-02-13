#ifndef AUDIOFILER_PLAYBACKCURSORGLOW_H
#define AUDIOFILER_PLAYBACKCURSORGLOW_H

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class PlaybackCursorGlow
 * @brief Provides helpers for rendering the playback cursor glow gradient.
 */
class PlaybackCursorGlow
{
public:
    static void renderGlow(juce::Graphics& g, const ControlPanel& controlPanel, const juce::Rectangle<int>& waveformBounds);
};


#endif // AUDIOFILER_PLAYBACKCURSORGLOW_H
