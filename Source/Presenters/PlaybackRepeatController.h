

#ifndef AUDIOFILER_PLAYBACKREPEATCONTROLLER_H
#define AUDIOFILER_PLAYBACKREPEATCONTROLLER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

class AudioPlayer;

class PlaybackRepeatController final : public SessionState::Listener,
                                       public juce::ChangeListener {
  public:
    PlaybackRepeatController(AudioPlayer &audioPlayerIn, SessionState &sessionStateIn);
    ~PlaybackRepeatController() override;

    void cutPreferenceChanged(const MainDomain::CutPreferences &) override;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

  private:
    void checkStateTransitions(bool autoPlayPreference, bool isPlaying);

    AudioPlayer &audioPlayer;
    SessionState &sessionState;
    bool lastIsPlaying = false;
    bool lastAutoPlayPreference = false;
};

#endif
