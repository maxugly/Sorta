#include "Presenters/PlaybackTimerManager.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "Presenters/PlaybackRepeatController.h"
#include "UI/InteractionCoordinator.h"
#include "Utils/Config.h"
#include "Utils/UIAnimationHelper.h"

PlaybackTimerManager::PlaybackTimerManager(SessionState &sessionStateIn, AudioPlayer &audioPlayerIn,
                                           InteractionCoordinator &coordinatorIn)
    : sessionState(sessionStateIn), audioPlayer(audioPlayerIn),
      interactionCoordinator(coordinatorIn) {
    startTimerHz(60);
}
PlaybackTimerManager::~PlaybackTimerManager() {
    stopTimer();
}

void PlaybackTimerManager::addListener(Listener *l) {
    const juce::ScopedLock lock(listenerLock);
    listeners.add(l);
}

void PlaybackTimerManager::removeListener(Listener *l) {
    const juce::ScopedLock lock(listenerLock);
    listeners.remove(l);
}

void PlaybackTimerManager::timerCallback() {
    const bool isZDown =
        juce::KeyPress::isKeyCurrentlyDown('z') || juce::KeyPress::isKeyCurrentlyDown('Z');

    const auto lastActivePoint = interactionCoordinator.getActiveZoomPoint();

    if (isZDown != m_isZKeyDown) {
        m_isZKeyDown = isZDown;
        if (m_isZKeyDown && m_zoomPointProvider) {
            interactionCoordinator.setActiveZoomPoint(m_zoomPointProvider());
        }
    }

    // Continuously sync manual zoom point when 'Z' is not overriding it
    if (!m_isZKeyDown) {
        interactionCoordinator.setActiveZoomPoint(interactionCoordinator.getManualZoomPoint());
    }

    const auto currentActivePoint = interactionCoordinator.getActiveZoomPoint();
    if (currentActivePoint != lastActivePoint) {
        listeners.call(&Listener::activeZoomPointChanged, currentActivePoint);

        if (currentActivePoint == AppEnums::ActiveZoomPoint::None) {
            if (interactionCoordinator.getNeedsJumpToCutIn()) {
                audioPlayer.setPlayheadPosition(sessionState.getCutIn());
                interactionCoordinator.setNeedsJumpToCutIn(false);
            }
        }
    }

    if (m_repeatController != nullptr)
        m_repeatController->tick();

    // Update master animation clock
    const float duration = Config::Animation::masterPhaseDurationSeconds;
    m_masterPhase += (1.0f / (60.0f * duration));
    if (m_masterPhase >= 1.0f)
        m_masterPhase = 0.0f;

    // Calculate breathing pulse at 1Hz (multiplier = duration since cycle is 'duration' seconds)
    m_breathingPulse = UIAnimationHelper::getSinePulse(m_masterPhase, duration);

    // Notify all high-frequency listeners
    const juce::ScopedLock lock(listenerLock);
    listeners.call(&Listener::playbackTimerTick);
    listeners.call(&Listener::animationUpdate, m_breathingPulse);
}
