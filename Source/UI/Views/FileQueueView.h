#pragma once
#include <JuceHeader.h>
#include <vector>

struct FileQueueViewState {
    std::vector<juce::String> upcomingFiles;
};

class FileQueueView : public juce::Component {
public:
    FileQueueView();
    ~FileQueueView() override;

    void paint(juce::Graphics& g) override;
    void updateState(const FileQueueViewState& newState);

private:
    FileQueueViewState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileQueueView)
};
