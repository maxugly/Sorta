#include "Presenters/RepeatButtonPresenter.h"
#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"

RepeatButtonPresenter::RepeatButtonPresenter(ControlPanel &ownerPanel, AudioPlayer &player,
                                             SessionState &state)
    : owner(ownerPanel), audioPlayer(player), sessionState(state) {
    sessionState.addListener(this);
}

RepeatButtonPresenter::~RepeatButtonPresenter() {
    sessionState.removeListener(this);
}

void RepeatButtonPresenter::initialiseButton(juce::TextButton &button) {
    repeatButton = &button;
    repeatButton->setClickingTogglesState(true);
    repeatButton->setToggleState(audioPlayer.isRepeating(), juce::dontSendNotification);
    repeatButton->onClick = [this] { audioPlayer.setRepeating(repeatButton->getToggleState()); };
}

void RepeatButtonPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
    juce::ignoreUnused(prefs);
    if (repeatButton != nullptr) {
        repeatButton->setToggleState(audioPlayer.isRepeating(), juce::dontSendNotification);
    }
}
