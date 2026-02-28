

#ifndef AUDIOFILER_PLAYBACKREPEATCONTROLLER_H
#define AUDIOFILER_PLAYBACKREPEATCONTROLLER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

class AudioPlayer;

/**
 * @file PlaybackRepeatController.h
 * @Source/Core/FileMetadata.h
 * @ingroup Logic
 * @brief Controller for managing automated playback repetition and auto-play logic.
 */

class AudioPlayer;

/**
 * @class PlaybackRepeatController
 * @brief Orchestrates automated playback loops and auto-play triggers.
 * 
 * @details Architecturally, this controller (acting as a specialized 
 *          presenter) monitors the SessionState (Model) for preference 
 *          changes and coordinates with the AudioPlayer to enforce 
 *          looping and automatic playback start on file load.
 * 
 * @see SessionState
 * @see ControlPanel
 * @see AudioPlayer
 */
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
