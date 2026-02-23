
#ifndef AUDIOFILER_ZOOMPRESENTER_H
#define AUDIOFILER_ZOOMPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "Core/AppEnums.h"

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

/**
 * @file ZoomPresenter.h
 * @ingroup Logic
 * @brief Coordinates zoom-related logic and synchronizes ZoomView state.
 * @details This presenter handles the high-frequency calculation of zoom popup bounds
 *          and time ranges, pushing them to the InteractionCoordinator and triggering
 *          view repaints.
 */
class ZoomPresenter final : public PlaybackTimerManager::Listener {
  public:
    explicit ZoomPresenter(ControlPanel& owner);
    ~ZoomPresenter() override;

    void playbackTimerTick() override;
    void animationUpdate(float breathingPulse) override;
    void activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) override;

  private:
    ControlPanel& owner;

    juce::Rectangle<int> lastPopupBounds;
    int lastMouseX{-1};
    int lastMouseY{-1};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomPresenter)
};

#endif
