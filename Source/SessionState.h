/*
  ==============================================================================

    SessionState.h
    Created: 17 Feb 2026
    Author:  Jules

  ==============================================================================
*/

#pragma once

#if defined(JUCE_HEADLESS)
    #include <juce_core/juce_core.h>
#else
    #include <JuceHeader.h>
#endif

struct SessionState
{
    bool autoplay = false;
    bool cutModeActive = false;
    bool shouldLoop = false;
    bool autoCutIn = true;
    bool autoCutOut = true;

    double cutIn = 0.0;
    double cutOut = 0.0;

    float thresholdIn = 0.15f;
    float thresholdOut = 0.15f;

    double getEffectiveDuration(double rawTotal, double start, double end) const
    {
        return cutModeActive ? juce::jmax(0.0, end - start) : rawTotal;
    }
};
