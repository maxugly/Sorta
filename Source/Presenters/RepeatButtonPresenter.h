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
 * @brief Manages the "Repeat/Loop" toggle button logic and state.
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
