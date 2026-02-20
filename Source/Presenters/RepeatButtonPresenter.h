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
 * @file Source/Presenters/RepeatButtonPresenter.h
 * @ingroup UI
 * @class RepeatButtonPresenter
 * @brief Manages the Repeat toggle button logic and state.
 * @details This presenter handles the interaction for the playback repeat/cycle button.
 *          It listens to `SessionState` for changes and updates the button's toggle state
 *          accordingly, and updates the `AudioPlayer` when the button is clicked.
 *
 * @see ControlPanel
 * @see AudioPlayer
 * @see SessionState
 */
class RepeatButtonPresenter : public SessionState::Listener
{
public:
    RepeatButtonPresenter(ControlPanel& ownerPanel, AudioPlayer& player, SessionState& state);
    ~RepeatButtonPresenter() override;

    void initialiseButton(juce::TextButton& button);

    // SessionState::Listener
    void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) override;

private:
    ControlPanel& owner;
    AudioPlayer& audioPlayer;
    SessionState& sessionState;
    juce::TextButton* repeatButton = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RepeatButtonPresenter)
};

#endif
