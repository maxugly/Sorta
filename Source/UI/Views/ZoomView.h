

#ifndef AUDIOFILER_ZOOMVIEW_H
#define AUDIOFILER_ZOOMVIEW_H

#if defined(JUCE_HEADLESS)
    #include <juce_gui_basics/juce_gui_basics.h>
#else
    #include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"

#include "Presenters/PlaybackTimerManager.h"

class ControlPanel;

class ZoomView : public juce::Component,
                 public PlaybackTimerManager::Listener
{
public:

    explicit ZoomView(ControlPanel& owner);
    ~ZoomView() override;

    void paint(juce::Graphics& g) override;

    void playbackTimerTick() override;
    void animationUpdate (float breathingPulse) override;
    void activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) override;

private:
    ControlPanel& owner;

    juce::Rectangle<int> lastPopupBounds;
    int lastMouseX{-1};
    int lastMouseY{-1};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomView)
};

#endif 
