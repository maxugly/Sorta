#pragma once
#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class DirectoryRoutingView : public juce::Component {
public:
    DirectoryRoutingView() = default;
    ~DirectoryRoutingView() override = default;
    
    void paint(juce::Graphics& g) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryRoutingView)
};
