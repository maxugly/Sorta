#include "PlaybackTimerManager.h"
#include "SessionState.h"
#include "AudioPlayer.h"

PlaybackTimerManager::PlaybackTimerManager(SessionState& sessionStateIn, AudioPlayer& audioPlayerIn)
    : sessionState(sessionStateIn), audioPlayer(audioPlayerIn)
{
    startTimerHz(60);
}

PlaybackTimerManager::~PlaybackTimerManager()
{
    stopTimer();
}

void PlaybackTimerManager::addListener(Listener* l)
{
    listeners.add(l);
}

void PlaybackTimerManager::removeListener(Listener* l)
{
    listeners.remove(l);
}

void PlaybackTimerManager::timerCallback()
{
    const bool isZDown = juce::KeyPress::isKeyCurrentlyDown('z') || juce::KeyPress::isKeyCurrentlyDown('Z');
    m_isZKeyDown = isZDown;

    // Notify all high-frequency listeners (Cursor, Zoom views, etc.)
    listeners.call(&Listener::playbackTimerTick);
}
