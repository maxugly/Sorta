#ifndef AUDIOFILER_BOUNDARYLOGICPRESENTER_H
#define AUDIOFILER_BOUNDARYLOGICPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"
#include "Core/SessionState.h"

/**
 * @file BoundaryLogicPresenter.h
 * @ingroup UI
 * @brief Presenter for Cut In/Out boundary text entry and validation.
 */

class ControlPanel;

/**
 * @class BoundaryLogicPresenter
 * @brief Manages the manual text entry and logic for Cut In/Out boundaries.
 * 
 * @details Architecturally, the BoundaryLogicPresenter adheres to the Model-View-Presenter (MVP) law, 
 *          serving as the "glue" between the text-based UI views (TextEditors) and the 
 *          underlying SessionState model. It encapsulates the complex logic required to 
 *          validate, format, and synchronize time-based input, ensuring that the 
 *          session state remains consistent regardless of user interactions. By handling 
 *          transient UI states like active editing or focus loss, it keeps the View 
 *          components "dumb" and focused purely on rendering.
 * 
 * @see SessionState, ControlPanel, CutPresenter
 */
class BoundaryLogicPresenter final : public juce::TextEditor::Listener,
                                 public juce::MouseListener,
                                 public SessionState::Listener {
  public:
    /**
     * @brief Constructs a new BoundaryLogicPresenter.
     * @param ownerPanel The parent ControlPanel containing the editors.
     * @param cutIn The TextEditor used for the 'In' point.
     * @param cutOut The TextEditor used for the 'Out' point.
     */
    BoundaryLogicPresenter(ControlPanel &ownerPanel, 
                           juce::TextEditor &cutIn, juce::TextEditor &cutOut);
    
    /** @brief Destructor. */
    ~BoundaryLogicPresenter() override;

    /** @brief Initialises the editors with default values and settings. */
    void initialiseEditors();

    /** @brief Refreshes the labels from the current SessionState values. */
    void refreshLabels();

    /** 
     * @brief Ensures that the 'In' point is always before the 'Out' point. 
     * @details This method enforces the structural integrity of the cut boundaries by 
     *          swapping or clamping values if a user tries to move the 'Out' point 
     *          behind the 'In' point.
     */
    void ensureCutOrder();

    /** 
     * @brief Triggered when the active audio file changes. 
     * @param filePath The path to the new file.
     */
    void fileChanged(const juce::String &filePath) override {
        juce::ignoreUnused(filePath);
        refreshLabels();
    }

    /** 
     * @brief Triggered when the 'In' point changes in the SessionState. 
     * @param position The new 'In' point position in seconds.
     */
    void cutInChanged(double) override { 
        ensureCutOrder(); 
        refreshLabels();
    }

    /** 
     * @brief Triggered when the 'Out' point changes in the SessionState. 
     * @param position The new 'Out' point position in seconds.
     */
    void cutOutChanged(double) override { 
        ensureCutOrder(); 
        refreshLabels();
    }

    /** 
     * @brief Triggered when cut preferences (e.g., snapping) change. 
     * @param prefs The new cut preferences.
     */
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override {
        refreshLabels();
    }

  private:
    /** @brief Handles text changes within the editors. */
    void textEditorTextChanged(juce::TextEditor &editor) override;

    /** @brief Handles the return key being pressed in an editor. */
    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;

    /** @brief Handles the escape key being pressed in an editor. */
    void textEditorEscapeKeyPressed(juce::TextEditor &editor) override;

    /** @brief Handles focus loss for an editor, typically triggering a commit. */
    void textEditorFocusLost(juce::TextEditor &editor) override;

    /** @brief Handles mouse down events for scrubbing or focusing. */
    void mouseDown(const juce::MouseEvent &event) override;

    /** @brief Handles mouse up events. */
    void mouseUp(const juce::MouseEvent &event) override;

    /** @brief Handles mouse entry into the editor area. */
    void mouseEnter(const juce::MouseEvent &event) override;

    /** @brief Handles mouse exit from the editor area. */
    void mouseExit(const juce::MouseEvent &event) override;

    /** @brief Handles mouse movement over the editor. */
    void mouseMove(const juce::MouseEvent &event) override;

    /** 
     * @brief Handles mouse wheel movement for fine-tuning positions. 
     * @details Implements a sensitivity-based adjustment where the scroll 
     *          amount translates to a delta in the cut position.
     */
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    /** 
     * @brief Syncs an editor's display text to a specific time position. 
     * @param editor The target TextEditor.
     * @param positionSeconds The time in seconds to display.
     */
    void syncEditorToPosition(juce::TextEditor &editor, double positionSeconds);

    /** 
     * @brief Validates and applies a new cut position from editor text. 
     * @param editor The source TextEditor.
     * @param newPosition The parsed position to apply.
     * @return True if the application was successful.
     */
    bool applyCutFromEditor(juce::TextEditor &editor, double newPosition);

    /** 
     * @brief Sets the cut position in the SessionState. 
     * @param isIn True if setting the 'In' point, false for 'Out'.
     * @param positionSeconds The new position in seconds.
     */
    void setCutPosition(bool isIn, double positionSeconds);

    /** @brief Returns the total length of the current audio file. */
    double getAudioTotalLength() const;

  private:
    ControlPanel &owner;
    juce::TextEditor &cutInEditor;
    juce::TextEditor &cutOutEditor;

    bool isEditingIn{false};
    bool isEditingOut{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoundaryLogicPresenter)
};

#endif
