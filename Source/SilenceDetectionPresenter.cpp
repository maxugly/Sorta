#include "SilenceDetectionPresenter.h"

#include "ControlPanel.h"
#include "SilenceDetector.h"
#include "AudioPlayer.h"

SilenceDetectionPresenter::SilenceDetectionPresenter(ControlPanel& ownerPanel)
    : owner(ownerPanel)
{
}

void SilenceDetectionPresenter::handleAutoCutInToggle(bool isActive)
{
    owner.silenceDetector->setIsAutoCutInActive(isActive);
    owner.updateComponentStates();
    if (isActive && hasLoadedAudio())
        owner.silenceDetector->detectInSilence();
}

void SilenceDetectionPresenter::handleAutoCutOutToggle(bool isActive)
{
    owner.silenceDetector->setIsAutoCutOutActive(isActive);
    owner.updateComponentStates();
    if (isActive && hasLoadedAudio())
        owner.silenceDetector->detectOutSilence();
}

bool SilenceDetectionPresenter::hasLoadedAudio() const
{
    return owner.getAudioPlayer().getThumbnail().getTotalLength() > 0.0;
}
