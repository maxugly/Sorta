#pragma once
#include "Core/SessionState.h"
#include "Workers/PreloadWorker.h"

class ControlPanel;
class FileQueueView;

class PreloadPresenter final : public SessionState::Listener {
public:
    PreloadPresenter(ControlPanel& ownerPanel, FileQueueView& view);
    ~PreloadPresenter() override;

    void queueChanged(const std::vector<juce::String>& newQueue) override;

private:
    ControlPanel& owner;
    FileQueueView& queueView;
    PreloadWorker preloadWorker; // Presenter owns its own worker

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreloadPresenter)
};
