#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Components/TransportButton.h"
#include "UI/Components/StyledTextEditor.h"

/**
 * @file CutLengthStrip.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief UI component for displaying and editing the total duration between cut points.
 */

/**
 * @class CutLengthStrip
 * @brief A specialized UI component for displaying and editing the total duration of the current cut.
 * 
 * @details Architecturally, the CutLengthStrip is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the duration-based text editor and 
 *          lock button. It relies entirely on its parent ControlPanel and 
 *          the CutLengthPresenter to handle duration calculations and state 
 *          updates. This decoupling ensures that the display of cut lengths 
 *          is handled cleanly without mixing in boundary validation logic.
 * 
 * @see CutLengthPresenter, ControlPanel, StyledTextEditor, TransportButton
 */
class CutLengthStrip : public juce::Component {
public:
    /** @brief Constructs a new CutLengthStrip and initialises its sub-components. */
    CutLengthStrip();

    /** @brief Standard JUCE paint callback. */
    void paint(juce::Graphics& g) override;

    /** @brief Standard JUCE resized callback, organizing the editor and lock button. */
    void resized() override;

    /** @return Reference to the duration-based text editor. */
    StyledTextEditor& getLengthEditor() { return lengthEditor; }

    /** @return Reference to the duration lock toggle button. */
    TransportButton& getLockButton() { return lockButton; }

private:
    StyledTextEditor lengthEditor;
    TransportButton lockButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLengthStrip)
};
