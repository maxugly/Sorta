#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Views/HintView.h"
#include "Core/SessionState.h"

class ControlPanel;

/**
 * @file HintPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for orchestrating contextual user hints.
 */

class ControlPanel;

/**
 * @class HintPresenter
 * @brief Observes mouse events and state changes to update the HintView.
 * 
 * @details Architecturally, this presenter acts as an aggregator of UI 
 *          context. It observes both the Model (SessionState) and various 
 *          View components (via MouseListener) to determine what helpful 
 *          text should be displayed to the user.
 * 
 * @see SessionState
 * @see ControlPanel
 * @see HintView
 */
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
