#pragma once
#include <JuceHeader.h>

/**
 * @file StyledTextEditor.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Customized JUCE TextEditor with application-specific styling and layout defaults.
 */

/**
 * @class StyledTextEditor
 * @brief A customized JUCE TextEditor with standardized application-specific styling and layout.
 * 
 * @details Architecturally, the StyledTextEditor is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to provide a consistent look and feel for 
 *          text entry across the application. It extends the standard JUCE 
 *          TextEditor to encapsulate common visual configurations—such as 
 *          justification, colors, and font styles—ensuring that the visual 
 *          presentation of data (like timecode or thresholds) is uniform.
 * 
 * @see ControlPanel, MarkerStrip, CutLengthStrip
 */
class StyledTextEditor : public juce::TextEditor {
public:
    /** @brief Constructs a new StyledTextEditor and applies default application styling. */
    StyledTextEditor();

    /** 
     * @brief Applies standard application visual styles to the editor. 
     * @param justification The text alignment to use.
     */
    void applyStandardStyle(juce::Justification justification = juce::Justification::centred);

    /** 
     * @brief Sets the text justification for the editor. 
     * @param newJustification The new alignment to apply.
     */
    void setJustification(juce::Justification newJustification) {
        justification = newJustification;
        juce::TextEditor::setJustification(newJustification);
    }

    /** @return The current text justification. */
    juce::Justification getJustification() const { return justification; }

    /** 
     * @brief Sets a custom text color for the editor. 
     * @param newColor The new color to apply.
     */
    void setCustomTextColor(juce::Colour newColor);

    /** @return The current custom text color. */
    juce::Colour getCustomTextColor() const;

private:
    juce::Justification justification = juce::Justification::centred;
};
