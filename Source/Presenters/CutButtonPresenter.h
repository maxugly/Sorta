

#ifndef AUDIOFILER_CUTBUTTONPRESENTER_H
#define AUDIOFILER_CUTBUTTONPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/InteractionCoordinator.h"

class ControlPanel;

class CutButtonPresenter : public InteractionCoordinator::Listener {
  public:
    explicit CutButtonPresenter(ControlPanel &ownerPanel);
    ~CutButtonPresenter() override;

    void updateColours();

    // InteractionCoordinator::Listener
    void placementModeChanged(AppEnums::PlacementMode newMode) override;
    void eyeCandyChanged(bool shouldShow) override;

  private:
    ControlPanel &owner;
    AppEnums::PlacementMode lastPlacementMode{AppEnums::PlacementMode::None};
};

#endif
