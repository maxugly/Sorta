#ifndef AUDIOFILER_PLAYBACKREPEATCONTROLLER_H
#define AUDIOFILER_PLAYBACKREPEATCONTROLLER_H

#include <JuceHeader.h>

class AudioPlayer;
class ControlPanel;

/**
 * @class PlaybackRepeatController
 * @brief Enforces repeat boundaries during playback.
 */
class PlaybackRepeatController final
{
public:
    PlaybackRepeatController(AudioPlayer& audioPlayerIn, ControlPanel& controlPanelIn);

    /**
     * @brief Runs a single repeat enforcement tick.
     */
    void tick();

private:
    AudioPlayer& audioPlayer;
    ControlPanel& controlPanel;
};

#endif // AUDIOFILER_PLAYBACKREPEATCONTROLLER_H
