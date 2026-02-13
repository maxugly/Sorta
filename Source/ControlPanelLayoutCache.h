#ifndef AUDIOFILER_CONTROLPANELLAYOUTCACHE_H
#define AUDIOFILER_CONTROLPANELLAYOUTCACHE_H

#include <JuceHeader.h>

/**
 * @class ControlPanelLayoutCache
 * @brief Stores geometry values computed during ControlPanel layout.
 */
struct ControlPanelLayoutCache
{
    juce::Rectangle<int> waveformBounds;
    juce::Rectangle<int> contentAreaBounds;
    int bottomRowTopY = 0;
    int playbackLeftTextX = 0;
    int playbackRightTextX = 0;
    int playbackCenterTextX = 0;
};


#endif // AUDIOFILER_CONTROLPANELLAYOUTCACHE_H
