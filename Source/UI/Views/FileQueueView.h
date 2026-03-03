#pragma once
#include <JuceHeader.h>
#include <vector>

struct FileQueueItem {
    juce::String filename;
    juce::String sizeStr;
    juce::String modifiedStr;
};

struct FileQueueViewState {
    std::vector<FileQueueItem> upcomingFiles;
};

class FileQueueContentView : public juce::Component {
public:
    FileQueueContentView();
    void paint(juce::Graphics& g) override;
    void updateState(const FileQueueViewState& newState);

private:
    FileQueueViewState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileQueueContentView)
};

class FileQueueView : public juce::Component {
public:
    FileQueueView();
    ~FileQueueView() override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void updateState(const FileQueueViewState& newState);

private:
    juce::Viewport viewport;
    FileQueueContentView content;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileQueueView)
};
