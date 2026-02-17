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
    owner.setAutoCutInActive(isActive);
    if (isActive && hasLoadedAudio())
        owner.silenceDetector->detectInSilence();
}

void SilenceDetectionPresenter::handleAutoCutOutToggle(bool isActive)
{
    owner.setAutoCutOutActive(isActive);
    if (isActive && hasLoadedAudio())
        owner.silenceDetector->detectOutSilence();
}

bool SilenceDetectionPresenter::hasLoadedAudio() const
{
    return owner.getAudioPlayer().getThumbnail().getTotalLength() > 0.0;
}
