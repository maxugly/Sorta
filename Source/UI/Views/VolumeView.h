#pragma once

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file VolumeView.h
 * @ingroup UI
 * @brief Passive view for the master volume control.
 */
class VolumeView final : public juce::Component {
  public:
    VolumeView();
    ~VolumeView() override = default;

    void resized() override;
    void paint(juce::Graphics &g) override;

    juce::Slider& getSlider() { return volumeKnob; }

  private:
    juce::Slider volumeKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeView)
};
