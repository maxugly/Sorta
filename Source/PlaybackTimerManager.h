#ifndef AUDIOFILER_PLAYBACKTIMERMANAGER_H
#define AUDIOFILER_PLAYBACKTIMERMANAGER_H

#if defined(JUCE_HEADLESS)
    #include <juce_core/juce_core.h>
    #include <juce_events/juce_events.h>
#else
    #include <JuceHeader.h>
#endif

#include "AppEnums.h"

class SessionState;
class AudioPlayer;

/**
 * @class PlaybackTimerManager
 * @brief A domain-level utility that manages high-frequency (60Hz) UI heartbeats.
 * 
 * This manager evacuates high-frequency polling from the UI layer. It monitors 
 * playback progress and keyboard state, notifying registered listeners at 60Hz.
 * 
 * @ingroup Logic
 */
class PlaybackTimerManager final : public juce::Timer
{
public:
    /**
     * @class Listener
     * @brief Interface for components that need high-frequency updates.
     */
    class Listener
    {
    public:
        virtual ~Listener() = default;

        /** @brief Called at a 60Hz frequency to trigger UI updates. */
        virtual void playbackTimerTick() = 0;
    };

    /**
     * @brief Constructor.
     * @param sessionStateIn Reference to the central application state.
     * @param audioPlayerIn Reference to the audio engine.
     */
    PlaybackTimerManager(SessionState& sessionStateIn, AudioPlayer& audioPlayerIn);
    
    /** @brief Destructor. stops the timer. */
    ~PlaybackTimerManager() override;

    /** @brief Registers a listener for timer ticks. */
    void addListener(Listener* l);

    /** @brief Unregisters a listener. */
    void removeListener(Listener* l);

    /** @brief Returns true if the 'z' key is currently held down. */
    bool isZKeyDown() const { return m_isZKeyDown; }

    /** @brief Internal timer callback. */
    void timerCallback() override;

private:
    SessionState& sessionState;
    AudioPlayer& audioPlayer;
    
    juce::ListenerList<Listener> listeners;
    
    bool m_isZKeyDown = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackTimerManager)
};

#endif 
