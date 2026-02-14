#ifndef AUDIOFILER_FOCUSMANAGER_H
#define AUDIOFILER_FOCUSMANAGER_H

#include <JuceHeader.h>

class ControlPanel;

enum class FocusTarget
{
    None,
    LoopIn,
    LoopOut,
    Playback,
    MouseManual
};

class FocusManager
{
public:
    explicit FocusManager(ControlPanel& owner);

    FocusTarget getCurrentTarget() const;
    double getFocusedTime() const;

    // Balanced multiplier values for unified scrolling logic
    static double getStepMultiplier(bool shift, bool ctrl);

private:
    ControlPanel& owner;
};

#endif // AUDIOFILER_FOCUSMANAGER_H
