#ifndef AUDIOFILER_PLAYBACKTIMERMANAGER_H
#define AUDIOFILER_PLAYBACKTIMERMANAGER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include <functional>

class SessionState;
class AudioPlayer;
class PlaybackRepeatController;
class InteractionCoordinator;

/**
 * @class PlaybackTimerManager
 * @brief A domain-level utility that manages high-frequency (60Hz) UI heartbeats.
 *
 * This manager evacuates high-frequency polling from the UI layer. It monitors
 * playback progress and keyboard state, notifying registered listeners at 60Hz.
 *
 * @ingroup Logic
 */
class PlaybackTimerManager final {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void playbackTimerTick() = 0;
        virtual void activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) {
            juce::ignoreUnused(newPoint);
        }
    };

    PlaybackTimerManager(SessionState &sessionStateIn, AudioPlayer &audioPlayerIn,
                         InteractionCoordinator &coordinatorIn);
    ~PlaybackTimerManager();

#if !defined(JUCE_HEADLESS)
    void attachToVBlank(juce::Component* component);
#endif

    void setZoomPointProvider(std::function<AppEnums::ActiveZoomPoint()> provider) {
        m_zoomPointProvider = std::move(provider);
    }

    void addListener(Listener *l);
    void removeListener(Listener *l);

    bool isZKeyDown() const {
        return m_isZKeyDown;
    }

    void onVBlank();

private:
    SessionState &sessionState;
    AudioPlayer &audioPlayer;
    InteractionCoordinator &interactionCoordinator;
    std::function<AppEnums::ActiveZoomPoint()> m_zoomPointProvider;
    juce::ListenerList<Listener> listeners;
    juce::CriticalSection listenerLock;
    bool m_isZKeyDown = false;

#if !defined(JUCE_HEADLESS)
    std::unique_ptr<juce::VBlankAttachment> vblankAttachment;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackTimerManager)
};
#endif
