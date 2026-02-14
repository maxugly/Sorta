#include "PlaybackHelpers.h"

double PlaybackHelpers::constrainPosition(double position, double loopIn, double loopOut)
{
    const double effectiveLoopIn = juce::jmin(loopIn, loopOut);
    const double effectiveLoopOut = juce::jmax(loopIn, loopOut);
    return juce::jlimit(effectiveLoopIn, effectiveLoopOut, position);
}
