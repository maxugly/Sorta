#ifndef AUDIOFILER_PLAYBACKOVERLAY_H
#define AUDIOFILER_PLAYBACKOVERLAY_H

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class PlaybackOverlay
 * @brief A transparent component that renders dynamic elements (cursors, overlays)
 *        on top of the waveform to avoid full repaints of the ControlPanel.
 */
class PlaybackOverlay : public juce::Component
{
public:
    explicit PlaybackOverlay(ControlPanel& owner);
    ~PlaybackOverlay() override = default;

    void paint(juce::Graphics& g) override;

private:
    ControlPanel& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackOverlay)
};

#endif // AUDIOFILER_PLAYBACKOVERLAY_H
