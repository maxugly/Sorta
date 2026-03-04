#include "UI/Views/VolumeView.h"
#include "Core/AppEnums.h"
#include "Utils/Config.h"

VolumeView::VolumeView() {
    addAndMakeVisible(volumeKnob);
    volumeKnob.setLookAndFeel(&knobLF);
    volumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeKnob.setRange(0.0, 1.0);

    volumeKnob.setColour(juce::Slider::rotarySliderFillColourId, Config::Colors::volumeKnobFill);
    volumeKnob.setColour(juce::Slider::rotarySliderOutlineColourId, Config::Colors::volumeKnobTrack);
    volumeKnob.setColour(juce::Slider::thumbColourId, Config::Colors::volumeKnobPointer);
}

VolumeView::~VolumeView() {
    volumeKnob.setLookAndFeel(nullptr);
}

void VolumeView::resized() {
    auto b = getLocalBounds();
    volumeKnob.setBounds(b.withSizeKeepingCentre(b.getHeight(), b.getHeight()));
}

void VolumeView::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat().reduced(Config::UI::ButtonOutlineThickness / 2.0f);
    auto cornerSize = Config::UI::ButtonCornerSize;

    // 1. Determine Corner Shapes based on GroupPosition
    const int pos = getProperties().getWithDefault("GroupPosition", (int)AppEnums::GroupPosition::Left);
    const bool roundLeft = (pos == (int)AppEnums::GroupPosition::Left || pos == (int)AppEnums::GroupPosition::Alone);
    const bool roundRight = (pos == (int)AppEnums::GroupPosition::Right || pos == (int)AppEnums::GroupPosition::Alone);

    juce::Path p;
    p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          cornerSize, cornerSize, roundLeft, roundRight, roundLeft, roundRight);

    g.setColour(Config::Colors::Button::base);
    g.fillPath(p);

    g.setColour(Config::Colors::Button::outline);
    g.strokePath(p, juce::PathStrokeType(Config::UI::ButtonOutlineThickness));
}
