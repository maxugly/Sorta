

#ifndef AUDIOFILER_CUTBUTTONPRESENTER_H
#define AUDIOFILER_CUTBUTTONPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"

class ControlPanel;

class CutButtonPresenter : public PlaybackTimerManager::Listener {
  public:
    explicit CutButtonPresenter(ControlPanel &ownerPanel);
    ~CutButtonPresenter() override;

    void updateColours();

    // PlaybackTimerManager::Listener
    void playbackTimerTick() override;
    void animationUpdate(float breathingPulse) override;

  private:
    ControlPanel &owner;
    AppEnums::PlacementMode lastPlacementMode{AppEnums::PlacementMode::None};
};

#endif
