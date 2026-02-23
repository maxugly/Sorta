#include "UI/Views/VolumeView.h"
#include "Utils/Config.h"

class KnobLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle,
                          const float rotaryEndAngle, juce::Slider &slider) override {
        auto outlineThickness = 2.0f;
        auto radius = (float)juce::jmin(width / 2, height / 2) - outlineThickness * 2.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Draw Knob Body (the circular part)
        g.setColour(Config::Colors::Button::outline.withAlpha(0.3f));
        g.fillEllipse(rx, ry, rw, rw);
        
        // 2. Draw Track (the full circle outline)
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse(rx, ry, rw, rw, outlineThickness);

        // 3. Draw Filled Area (the arc from start to current position)
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, juce::PathStrokeType(outlineThickness * 2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 4. Draw Pointer/Notch
        juce::Path p;
        auto pointerLength = radius * 0.4f;
        auto pointerThickness = 3.0f;
        // Pointer at the edge
        p.addRoundedRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength, 1.0f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

static KnobLookAndFeel knobLF;

VolumeView::VolumeView() {
    addAndMakeVisible(volumeKnob);
    volumeKnob.setLookAndFeel(&knobLF);
    volumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeKnob.setRange(0.0, 1.0);

    volumeKnob.setColour(juce::Slider::rotarySliderFillColourId, Config::Colors::volumeKnobFill);
    volumeKnob.setColour(juce::Slider::rotarySliderOutlineColourId, Config::Colors::volumeKnobTrack);
    volumeKnob.setColour(juce::Slider::thumbColourId, juce::Colours::white);
}

void VolumeView::resized() {
    auto b = getLocalBounds();
    // Fill the whole height (32px) and center horizontally in the 80px container
    volumeKnob.setBounds(b.withSizeKeepingCentre(b.getHeight(), b.getHeight()));
}

void VolumeView::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds().toFloat().reduced(Config::UI::ButtonOutlineThickness / 2.0f);
    auto cornerSize = Config::UI::ButtonCornerSize;

    g.setColour(Config::Colors::Button::base);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(Config::Colors::Button::outline);
    g.drawRoundedRectangle(bounds, cornerSize, Config::UI::ButtonOutlineThickness);
}
