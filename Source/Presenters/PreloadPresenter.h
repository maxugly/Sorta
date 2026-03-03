#pragma once
#include "Core/SessionState.h"

class ControlPanel;
class FileQueueView;

class PreloadPresenter final : public SessionState::Listener {
public:
    PreloadPresenter(ControlPanel& ownerPanel, FileQueueView& view);
    ~PreloadPresenter() override;

private:
    ControlPanel& owner;
    FileQueueView& queueView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreloadPresenter)
};
