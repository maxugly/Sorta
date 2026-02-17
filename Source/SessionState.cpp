#include "SessionState.h"
#include "Config.h"

SessionState::SessionState() {
    cutPrefs.active = false;
    cutPrefs.autoCut.inActive = false;
    cutPrefs.autoCut.outActive = false;
    cutPrefs.autoCut.thresholdIn = Config::Audio::silenceThresholdIn;
    cutPrefs.autoCut.thresholdOut = Config::Audio::silenceThresholdOut;

    isLooping = true;
    autoplay = true;
}
