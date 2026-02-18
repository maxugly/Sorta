#include "SilenceDetector.h"
#include "ControlPanel.h"
#include "AudioPlayer.h"
#include "SilenceThresholdPresenter.h"

SilenceDetector::SilenceDetector(ControlPanel& ownerPanel)
    : owner(ownerPanel),
      currentInSilenceThreshold(Config::Audio::silenceThresholdIn),
      currentOutSilenceThreshold(Config::Audio::silenceThresholdOut)
{
    thresholdPresenter = std::make_unique<SilenceThresholdPresenter>(*this, owner);
}

SilenceDetector::~SilenceDetector() = default;

void SilenceDetector::detectInSilence()
{
    owner.getAudioPlayer().startSilenceAnalysis(currentInSilenceThreshold, true);
}

void SilenceDetector::detectOutSilence()
{
    owner.getAudioPlayer().startSilenceAnalysis(currentOutSilenceThreshold, false);
}
