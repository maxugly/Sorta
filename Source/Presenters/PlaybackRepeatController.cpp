

#include "Presenters/PlaybackRepeatController.h"

#include "Core/AudioPlayer.h"

PlaybackRepeatController::PlaybackRepeatController(AudioPlayer &audioPlayerIn,
                                                   SessionState &sessionStateIn)
    : audioPlayer(audioPlayerIn), sessionState(sessionStateIn) {
    sessionState.addListener(this);
    audioPlayer.addChangeListener(this);
}

PlaybackRepeatController::~PlaybackRepeatController() {
    audioPlayer.removeChangeListener(this);
    sessionState.removeListener(this);
}

void PlaybackRepeatController::cutPreferenceChanged(const MainDomain::CutPreferences &) {
    checkStateTransitions(sessionState.getCutPrefs().autoplay, audioPlayer.isPlaying());
}

void PlaybackRepeatController::changeListenerCallback(juce::ChangeBroadcaster *) {
    checkStateTransitions(sessionState.getCutPrefs().autoplay, audioPlayer.isPlaying());
}

void PlaybackRepeatController::checkStateTransitions(bool autoPlayPreference, bool isPlaying) {
    // Only take action if the preference has changed OR if playback state changed
    if (autoPlayPreference != lastAutoPlayPreference || isPlaying != lastIsPlaying) {
        if (autoPlayPreference && !lastAutoPlayPreference && !isPlaying) {
            // Transition: User just enabled AutoPlay while NOT playing -> Start immediately
            audioPlayer.startPlayback();
        } else if (autoPlayPreference && !isPlaying && lastIsPlaying) {
            // Transition: AutoPlay was on, and we JUST stopped (naturally or manual stop)
            // In CD-style transport, manual Stop or end-of-track kills AutoPlay
            sessionState.setAutoPlayActive(false);
        }
    }

    lastIsPlaying = isPlaying;
    lastAutoPlayPreference = autoPlayPreference;
}
