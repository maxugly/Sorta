#pragma once

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Utils/Config.h"

/**
 * @file VolumeView.h
 * @ingroup UI
 * @brief Passive view for the master volume control.
 */

/**
 * @class KnobLookAndFeel
 * @brief Custom LookAndFeel for the volume knob.
 */
class KnobLookAndFeel final : public juce::LookAndFeel_V4 {
  public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle,
                          const float rotaryEndAngle, juce::Slider &slider) override {
        const auto outlineThickness = Config::Layout::VolumeKnob::outlineThickness;
        const auto radius = (float)juce::jmin(width / 2, height / 2) - outlineThickness * 2.0f;
        const auto centreX = (float)x + (float)width * 0.5f;
        const auto centreY = (float)y + (float)height * 0.5f;
        const auto rx = centreX - radius;
        const auto ry = centreY - radius;
        const auto rw = radius * 2.0f;
        const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Draw Knob Body
        g.setColour(Config::Colors::Button::base);
        g.fillEllipse(rx, ry, rw, rw);
        
        // 2. Draw Track
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.drawEllipse(rx, ry, rw, rw, outlineThickness);

        // 3. Draw Filled Area
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, juce::PathStrokeType(outlineThickness * 2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 4. Draw Pointer
        juce::Path p;
        const auto pointerLength = radius * Config::Layout::VolumeKnob::pointerLengthProportion;
        const auto pointerThickness = Config::Layout::VolumeKnob::pointerThickness;
        p.addRoundedRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength, 1.0f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(Config::Colors::volumeKnobPointer);
        g.fillPath(p);
    }
};

/**
 * @class VolumeView
 * @brief A passive UI component for the master volume knob.
 * 
 * @details Architecturally, VolumeView adheres to the Model-View-Presenter (MVP) law 
 *          as a "Passive View." it contains zero business logic and only handles 
 *          rendering and basic JUCE component lifecycle methods like paint() 
 *          and resized(). It relies entirely on the VolumePresenter to 
 *          synchronize its state with the underlying audio engine and SessionState.
 * 
 * @see VolumePresenter
 */
class VolumeView final : public juce::Component {
  public:
    VolumeView();
    ~VolumeView() override;

    void resized() override;
    void paint(juce::Graphics &g) override;

    juce::Slider& getSlider() { return volumeKnob; }

  private:
    KnobLookAndFeel knobLF;
    juce::Slider volumeKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeView)
};
