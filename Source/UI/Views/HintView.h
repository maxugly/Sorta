#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * @file HintView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Passive view for displaying contextual hints and feedback.
 */

/**
 * @class HintView
 * @brief A passive UI component for displaying textual hints and status messages.
 * 
 * @details Architecturally, HintView is a "Passive View" in the Model-View-Presenter (MVP) 
 *          hierarchy. It is responsible for nothing more than painting the 
 *          text provided by its presenter. It contains no decision-making 
 *          logic regarding what hint to show or when to show it.
 * 
 * @see HintPresenter
 */
class HintView : public juce::Component {
public:
    HintView();
    void setHint(const juce::String& text);
    void paint(juce::Graphics& g) override;
private:
    juce::String currentHint;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HintView)
};
