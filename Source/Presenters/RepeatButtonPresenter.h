#ifndef AUDIOFILER_REPEATBUTTONPRESENTER_H
#define AUDIOFILER_REPEATBUTTONPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

class ControlPanel;
class AudioPlayer;

/**
 * @class RepeatButtonPresenter
 * @brief Manages the "Repeat" toggle button logic and state.
 */
/**
 * @file RepeatButtonPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for managing the repeat/loop toggle button.
 */

class ControlPanel;
class AudioPlayer;

/**
 * @class RepeatButtonPresenter
 * @brief Manages the visual and logical state of the transport's repeat button.
 * 
 * @details Architecturally, this presenter synchronizes the loop preference 
 *          in the SessionState (Model) with the Repeat button in the 
 *          TransportStrip (View). It ensures the button reflects the 
 *          active loop state and dispatches user clicks back to the Model.
 * 
 * @see SessionState
 * @see ControlPanel
 */
class RepeatButtonPresenter : public SessionState::Listener {
  public:
    RepeatButtonPresenter(ControlPanel &ownerPanel, AudioPlayer &player, SessionState &state);
    ~RepeatButtonPresenter() override;

    void initialiseButton(juce::TextButton &button);

    // SessionState::Listener
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

  private:
    ControlPanel &owner;
    AudioPlayer &audioPlayer;
    SessionState &sessionState;
    juce::TextButton *repeatButton = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RepeatButtonPresenter)
};

#endif
