#include "TransportButton.h"

TransportButton::TransportButton(const juce::String &name) : juce::TextButton(name) {
}

void TransportButton::mouseDown(const juce::MouseEvent &event) {
    lastClickWasRight = event.mods.isRightButtonDown();
    juce::TextButton::mouseDown(event);
}

void TransportButton::mouseUp(const juce::MouseEvent &event) {
    if (isEnabled() && getLocalBounds().contains(event.getPosition())) {
        if (event.mouseWasClicked()) {
            if (lastClickWasRight) {
                if (onRightClick)
                    onRightClick();
            } else {
                if (onLeftClick)
                    onLeftClick();
            }
        }
    }

    juce::TextButton::mouseUp(event);
}
