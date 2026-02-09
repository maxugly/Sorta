#pragma once

#include <JuceHeader.h>
#include "Config.h"

namespace ControlPanelCopy
{
inline const juce::String& openButtonText() { return Config::openButtonText; }
inline const juce::String& playButtonText() { return Config::playButtonText; }
inline const juce::String& stopButtonText() { return Config::stopButtonText; }
inline const juce::String& viewModeClassicText() { return Config::viewModeClassicText; }
inline const juce::String& viewModeOverlayText() { return Config::viewModeOverlayText; }
inline const juce::String& channelViewMonoText() { return Config::channelViewMonoText; }
inline const juce::String& channelViewStereoText() { return Config::channelViewStereoText; }
inline const juce::String& qualityButtonText() { return Config::qualityButtonText; }
inline const juce::String& qualityHighText() { return Config::qualityHighText; }
inline const juce::String& qualityMediumText() { return Config::qualityMediumText; }
inline const juce::String& qualityLowText() { return Config::qualityLowText; }
inline const juce::String& exitButtonText() { return Config::exitButtonText; }
inline const juce::String& statsButtonText() { return Config::statsButtonText; }
inline const juce::String& loopButtonText() { return Config::loopButtonText; }
inline const juce::String& autoplayButtonText() { return Config::autoplayButtonText; }
inline const juce::String& autoCutInButtonText() { return Config::autoCutInButtonText; }
inline const juce::String& autoCutOutButtonText() { return Config::autoCutOutButtonText; }
inline const juce::String& cutButtonText() { return Config::cutButtonText; }
inline const juce::String& loopInButtonText() { return Config::loopInButtonText; }
inline const juce::String& loopOutButtonText() { return Config::loopOutButtonText; }
inline const juce::String& clearButtonText() { return Config::clearButtonText; }

inline const juce::String& silenceThresholdInTooltip()
{
    static const juce::String text("Threshold to detect start of sound (0.0 - 1.0)");
    return text;
}

inline const juce::String& silenceThresholdOutTooltip()
{
    static const juce::String text("Threshold to detect end of sound (0.0 - 1.0)");
    return text;
}
} // namespace ControlPanelCopy

