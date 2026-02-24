#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Views/HintView.h"
#include "Core/SessionState.h"

class ControlPanel;

class HintPresenter : public juce::MouseListener, public SessionState::Listener {
public:
    HintPresenter(ControlPanel& ownerPanel, HintView& view);
    ~HintPresenter() override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    void viewModeChanged(AppEnums::ViewMode newMode) override;
    void channelViewModeChanged(AppEnums::ChannelViewMode newMode) override;
private:
    ControlPanel& owner;
    HintView& hintView;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HintPresenter)
};
