

#include "Workers/SilenceDetector.h"
#include "Core/AudioPlayer.h"
#include "Presenters/SilenceThresholdPresenter.h"
#include "UI/ControlPanel.h"

SilenceDetector::SilenceDetector(ControlPanel &ownerPanel)
    : owner(ownerPanel), currentInSilenceThreshold(Config::Audio::silenceThresholdIn),
      currentOutSilenceThreshold(Config::Audio::silenceThresholdOut) {
    thresholdPresenter = std::make_unique<SilenceThresholdPresenter>(*this, owner);
}

SilenceDetector::~SilenceDetector() = default;

