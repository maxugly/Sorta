#include "TransportPresenter.h"

#include "ControlPanel.h"
#include "AudioPlayer.h"

TransportPresenter::TransportPresenter(ControlPanel& ownerPanel)
    : owner(ownerPanel)
{
}

void TransportPresenter::handleLoopToggle(bool shouldLoop)
{
    owner.setShouldLoop(shouldLoop);
    owner.getAudioPlayer().setLooping(owner.getShouldLoop());
}

void TransportPresenter::handleAutoplayToggle(bool shouldAutoplay)
{
    owner.m_shouldAutoplay = shouldAutoplay;
}

void TransportPresenter::handleCutModeToggle(bool enableCutMode)
{
    owner.m_isCutModeActive = enableCutMode;
    owner.updateComponentStates();
    if (owner.m_isCutModeActive && owner.getAudioPlayer().isPlaying())
        enforceCutLoopBounds();
}

void TransportPresenter::enforceCutLoopBounds() const
{
    auto& audioPlayer = owner.getAudioPlayer();
    auto& transport = audioPlayer.getTransportSource();
    const double currentPosition = transport.getCurrentPosition();
    const double loopIn = owner.getLoopInPosition();
    const double loopOut = owner.getLoopOutPosition();

    if (loopOut > loopIn
        && (currentPosition < loopIn || currentPosition >= loopOut))
    {
        transport.setPosition(loopIn);
    }
}
