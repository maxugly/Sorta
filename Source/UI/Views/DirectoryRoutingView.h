#pragma once
#include <JuceHeader.h>

class DirectoryRoutingView : public juce::Component {
public:
    DirectoryRoutingView() = default;
    ~DirectoryRoutingView() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryRoutingView)
};
