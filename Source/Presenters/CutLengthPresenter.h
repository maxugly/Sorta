#ifndef AUDIOFILER_CUTLENGTHPRESENTER_H
#define AUDIOFILER_CUTLENGTHPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

/**
 * @file CutLengthPresenter.h
 * @ingroup UI
 * @brief Presenter for calculating and displaying the total duration between cut boundaries.
 */

class ControlPanel;

/**
 * @class CutLengthPresenter
 * @brief Manages the logic for displaying and editing the total duration of the current cut.
 * 
 * @details Following the Model-View-Presenter (MVP) law, the CutLengthPresenter serves as 
 *          the "glue" between the SessionState (Model) and the duration-specific UI 
 *          views (TextEditors). It calculates the delta between 'In' and 'Out' 
 *          points and provides a mechanism for users to adjust the total length 
 *          by modifying the 'Out' point relative to the 'In' point. By handling 
 *          transient states like active duration editing, it keeps the View 
 *          components focused on simple text rendering.
 * 
 * @see SessionState, BoundaryLogicPresenter, ControlPanel
 */
class CutLengthPresenter : public juce::TextEditor::Listener,
                           public juce::MouseListener,
                           public SessionState::Listener {
public:
    /**
     * @brief Constructs a new CutLengthPresenter.
     * @param ownerPanel The parent ControlPanel containing the duration editor.
     */
    explicit CutLengthPresenter(ControlPanel& ownerPanel);

    /** @brief Destructor. */
    ~CutLengthPresenter() override;

    /** @brief Initialises the editor with default values and settings. */
    void initialiseEditors();

    /** @brief Updates the editor text based on the current cut duration. */
    void updateEditor();

    /** 
     * @brief Triggered when the active audio file changes. 
     * @param filePath The path to the new file.
     */
    void fileChanged(const juce::String& filePath) override {
        juce::ignoreUnused(filePath);
        updateEditor();
    }

    /** 
     * @brief Triggered when the 'In' point changes in the SessionState. 
     * @param value The new 'In' point position.
     */
    void cutInChanged(double) override { updateEditor(); }

    /** 
     * @brief Triggered when the 'Out' point changes in the SessionState. 
     * @param value The new 'Out' point position.
     */
    void cutOutChanged(double) override { updateEditor(); }

    /** 
     * @brief Triggered when cut preferences (e.g., snapping) change. 
     * @param prefs The new cut preferences.
     */
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override { updateEditor(); }

private:
    /** @brief Handles text changes within the editor. */
    void textEditorTextChanged(juce::TextEditor& editor) override;

    /** @brief Handles the return key being pressed in the editor, committing the change. */
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;

    /** @brief Handles the escape key being pressed in the editor, cancelling the edit. */
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;

    /** @brief Handles focus loss for the editor, typically triggering a commit. */
    void textEditorFocusLost(juce::TextEditor& editor) override;

    /** 
     * @brief Handles mouse wheel movement for fine-tuning the cut length. 
     * @details Translates scroll deltas into time adjustments, keeping the 
     *          'In' point fixed and moving the 'Out' point.
     */
    void mouseWheelMove(const juce::MouseEvent& event,
                        const juce::MouseWheelDetails& wheel) override;

    /** @brief Handles mouse up events. */
    void mouseUp(const juce::MouseEvent& event) override;

    /** @brief Handles mouse movement over the editor. */
    void mouseMove(const juce::MouseEvent& event) override;

    /** @brief Handles mouse down events for scrubbing or focusing. */
    void mouseDown(const juce::MouseEvent& event) override;

    /** 
     * @brief Parses the editor text and applies the new duration to the SessionState. 
     * @param editor The target TextEditor.
     */
    void applyTimeEdit(juce::TextEditor& editor);

    ControlPanel& owner;
    bool isEditingCutLength{ false };
};

#endif
