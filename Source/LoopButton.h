#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class LoopButton : public juce::TextButton
{
public:
    std::function<void()> onLeftClick;
    std::function<void()> onRightClick;

    LoopButton (const juce::String& name) : juce::TextButton (name) {}
    ~LoopButton() override = default;

private:
    void mouseUp (const juce::MouseEvent& event) override
    {
        if (isEnabled() && event.mods.isRightButtonDown())
        {
            if (onRightClick)
                onRightClick();
        }
        else if (isEnabled() && event.mods.isLeftButtonDown())
        {
            if (onLeftClick)
                onLeftClick();
        }

        juce::TextButton::mouseUp(event); // Call base class method
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopButton)
};
