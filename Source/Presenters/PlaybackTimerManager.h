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

/**
 * @file PlaybackTimerManager.h
 * @ingroup Logic
 * @brief Domain-level utility that manages high-frequency (60Hz) UI heartbeats.
 */

class SessionState;
class AudioPlayer;
class PlaybackRepeatController;
class InteractionCoordinator;

/**
 * @class PlaybackTimerManager
 * @brief A domain-level utility that manages high-frequency (60Hz) UI heartbeats.
 *
 * @details Architecturally, the PlaybackTimerManager serves as the central 
 *          clock or "heartbeat" for the application's visual synchronization. 
 *          It decouples high-frequency polling from individual UI components, 
 *          ensuring that playhead updates, waveform animations, and real-time 
 *          HUD changes occur at a consistent 60Hz. By centralizing the timer, 
 *          it minimizes CPU overhead and ensures that all "active" presenters 
 *          (following the MVP law) remain perfectly in sync with the underlying 
 *          audio playback engine. It also manages transient input states, like 
 *          detecting held keys for modifier-based interactions.
 *
 * @see SessionState, AudioPlayer, InteractionCoordinator, CutPresenter, ZoomPresenter
 */
class PlaybackTimerManager final {
public:
    /** 
     * @class Listener
     * @brief Listener interface for receiving high-frequency heartbeats.
     */
    class Listener {
    public:
        virtual ~Listener() = default;

        /** @brief Triggered on every timer tick (approx. 60Hz). */
        virtual void playbackTimerTick() = 0;

        /** 
         * @brief Triggered when the active zoom point changes. 
         * @param newPoint The new focal point for zooming.
         */
        virtual void activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) {
            juce::ignoreUnused(newPoint);
        }
    };

    /**
     * @brief Constructs a new PlaybackTimerManager.
     * @param sessionStateIn Reference to the application session state.
     * @param audioPlayerIn Reference to the audio engine for tracking playhead progress.
     * @param coordinatorIn Reference to the InteractionCoordinator for sync logic.
     */
    PlaybackTimerManager(SessionState &sessionStateIn, AudioPlayer &audioPlayerIn,
                         InteractionCoordinator &coordinatorIn);

    /** @brief Destructor. */
    ~PlaybackTimerManager();

#if !defined(JUCE_HEADLESS)
    /** 
     * @brief Attaches the timer to the system's VBlank for vsync-synchronized updates. 
     * @param component The component used to obtain the VBlank attachment.
     */
    void attachToVBlank(juce::Component* component);
#endif

    /** 
     * @brief Sets a callback that provides the currently active zoom point. 
     * @param provider The provider function.
     */
    void setZoomPointProvider(std::function<AppEnums::ActiveZoomPoint()> provider) {
        m_zoomPointProvider = std::move(provider);
    }

    /** @brief Adds a listener to be notified on every tick. */
    void addListener(Listener *l);

    /** @brief Removes a previously registered listener. */
    void removeListener(Listener *l);

    /** @return True if the 'Z' key is currently held down. */
    bool isZKeyDown() const {
        return m_isZKeyDown;
    }

    /** @brief Internal callback for VBlank or timer triggers. */
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
