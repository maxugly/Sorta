#pragma once
#include <JuceHeader.h>
#include <array>

struct DirectoryRoutingViewState {
    std::array<juce::String, 10> destinations;
    int lastRoutedIndex{-1}; // -1 means no active highlight
};

class DirectoryRoutingView : public juce::Component {
public:
    DirectoryRoutingView();
    ~DirectoryRoutingView() override = default;

    void paint(juce::Graphics& g) override;
    void updateState(const DirectoryRoutingViewState& newState);

private:
    DirectoryRoutingViewState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryRoutingView)
};
