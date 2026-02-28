

#ifndef AUDIOFILER_CUTBUTTONPRESENTER_H
#define AUDIOFILER_CUTBUTTONPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/InteractionCoordinator.h"

class ControlPanel;

/**
 * @file CutButtonPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for managing the visual state and placement logic of the Cut button.
 */

class ControlPanel;

/**
 * @class CutButtonPresenter
 * @brief Manages the interactive behavior and coloring of the main Cut engagement button.
 * 
 * @details Architecturally, this presenter synchronizes the Cut button's 
 *          appearance with the current interaction mode (In/Out placement). 
 *          It acts as an observer of the InteractionCoordinator to ensure 
 *          the View reflects the active logical state.
 * 
 * @see SessionState
 * @see ControlPanel
 * @see InteractionCoordinator
 */
class CutButtonPresenter : public InteractionCoordinator::Listener {
  public:
    explicit CutButtonPresenter(ControlPanel &ownerPanel);
    ~CutButtonPresenter() override;

    void updateColours();

    // InteractionCoordinator::Listener
    void placementModeChanged(AppEnums::PlacementMode newMode) override;

  private:
    ControlPanel &owner;
    AppEnums::PlacementMode lastPlacementMode{AppEnums::PlacementMode::None};
};

#endif
