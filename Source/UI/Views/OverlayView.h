#ifndef AUDIOFILER_OVERLAYVIEW_H
#define AUDIOFILER_OVERLAYVIEW_H

#if defined(JUCE_HEADLESS)
    #include <juce_gui_basics/juce_gui_basics.h>
#else
    #include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"

class ControlPanel;

/**
 * @class OverlayView
 * @brief A transparent layer for rendering eye-candy (connecting lines, group outlines).
 * 
 * This view sits at the top of the ControlPanel stack and reacts to animation heartbeats.
 */
class OverlayView : public juce::Component,
                    public PlaybackTimerManager::Listener
{
public:
    explicit OverlayView(ControlPanel& owner);
    ~OverlayView() override;

    void paint(juce::Graphics& g) override;

    // PlaybackTimerManager::Listener
    void playbackTimerTick() override {}
    void animationUpdate(float breathingPulse) override;

private:
    ControlPanel& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlayView)
};

#endif
