#pragma once

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Components/StyledTextEditor.h"
#include "UI/Components/TransportButton.h"

/**
 * @file BottomPanelView.h
 * @ingroup UI
 * @brief Passive view for the bottom panel.
 * 
 * @details Architecturally, BottomPanelView adheres to the Model-View-Presenter (MVP) law 
 *          as a "Passive View." It contains zero business logic and only handles 
 *          rendering and basic JUCE component lifecycle methods. It serves as a 
 *          container for bottom-aligned UI elements.
 */
class BottomPanelView final : public juce::Component {
public:
    BottomPanelView();
    ~BottomPanelView() override = default;

    void resized() override;

    TransportButton openButton;

private:
    StyledTextEditor placeholderEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BottomPanelView)
};
