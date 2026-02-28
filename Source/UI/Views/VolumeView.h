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
    ~VolumeView() override = default;

    void resized() override;
    void paint(juce::Graphics &g) override;

    juce::Slider& getSlider() { return volumeKnob; }

  private:
    juce::Slider volumeKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeView)
};
