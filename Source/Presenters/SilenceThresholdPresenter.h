#ifndef AUDIOFILER_SILENCETHRESHOLDPRESENTER_H
#define AUDIOFILER_SILENCETHRESHOLDPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file SilenceThresholdPresenter.h
 * @ingroup Logic
 * @brief Presenter for silence detection threshold controls.
 */

class ControlPanel;

/**
 * @class SilenceThresholdPresenter
 * @brief Manages the logic for silence detection threshold text entry and validation.
 * 
 * @details Architecturally, the SilenceThresholdPresenter adheres to the Model-View-Presenter (MVP) law, 
 *          serving as the "glue" between the text-based UI views (TextEditors) and the 
 *          underlying silence analysis parameters in the SessionState. It ensures that 
 *          threshold values remain within a valid percentage range (1-99%) and 
 *          synchronizes these values across the UI. By managing transient editing 
 *          states and focus transitions, it keeps the View components focused 
 *          purely on text rendering.
 * 
 * @see SessionState, SilenceAnalysisWorker, ControlPanel
 */
class SilenceThresholdPresenter final : private juce::TextEditor::Listener,
                                        public juce::MouseListener {
  public:
    /**
     * @brief Constructs a new SilenceThresholdPresenter.
     * @param ownerPanel The parent ControlPanel.
     * @param inEditor The editor for the 'In' threshold.
     * @param outEditor The editor for the 'Out' threshold.
     */
    SilenceThresholdPresenter(ControlPanel &ownerPanel, juce::TextEditor &inEditor, juce::TextEditor &outEditor);

    /** @brief Destructor. */
    ~SilenceThresholdPresenter() override;

  private:
    /** 
     * @brief Configures an editor with initial values and visual properties. 
     * @param editor The TextEditor to configure.
     * @param initialValue The starting percentage value.
     * @param tooltip The tooltip text for the editor.
     */
    void configureEditor(juce::TextEditor &editor, float initialValue, const juce::String &tooltip);

    /** @brief Handles text changes within the editors. */
    void textEditorTextChanged(juce::TextEditor &editor) override;

    /** @brief Handles the return key being pressed, committing the threshold value. */
    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;

    /** @brief Handles focus loss, typically triggering a commit of the threshold. */
    void textEditorFocusLost(juce::TextEditor &editor) override;

    /** 
     * @brief Handles mouse wheel movement for fine-tuning thresholds. 
     * @details Translates scroll deltas into percentage increments/decrements.
     */
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    /** 
     * @brief Commits the threshold value from the editor to the state. 
     * @param editor The source TextEditor.
     */
    void applyThresholdFromEditor(juce::TextEditor &editor);

    /** 
     * @brief Updates the threshold in the state only if the editor content is valid. 
     * @param editor The source TextEditor.
     */
    void updateThresholdFromEditorIfValid(juce::TextEditor &editor);

    /** 
     * @brief Restores the editor text to the current valid state value. 
     * @param editor The target TextEditor.
     */
    void restoreEditorToCurrentValue(juce::TextEditor &editor);

    /** @brief Validates if a percentage value is within the 1-99 range. */
    bool isValidPercentage(int value) const noexcept {
        return value >= 1 && value <= 99;
    }

    ControlPanel &owner;
    juce::TextEditor &inThresholdEditor;
    juce::TextEditor &outThresholdEditor;
};

#endif
