#ifndef AUDIOFILER_PLAYBACKTEXTPRESENTER_H
#define AUDIOFILER_PLAYBACKTEXTPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"
#include "Core/SessionState.h"

class ControlPanel;

/**
 * @file PlaybackTextPresenter.h
 * @ingroup Logic
 * @brief Presenter responsible for synchronizing the playhead position with its text representation.
 * 
 * @details Architecturally, PlaybackTextPresenter acts as the "Presenter Glue" between 
 *          the `AudioPlayer` (via `SessionState`) and the `StyledTextEditor` views. 
 *          It implements the MVP (Model-View-Presenter) Law by translating 
 *          high-precision double seconds into formatted HH:MM:SS.ms strings and 
 *          vice-versa.
 * 
 *          Key responsibilities:
 *          - **Real-time Sync**: Observes the `PlaybackTimerManager` to update the 
 *            "Elapsed" and "Remaining" time displays at a consistent 15Hz-30Hz rate.
 *          - **Bidirectional Editing**: Allows the user to manually type a time or 
 *            "scrub" values using the mouse wheel, instantly updating the 
 *            `AudioPlayer`'s playhead position.
 *          - **Contextual Formatting**: Dynamically switches text colors based on 
 *            the validity of the entered time relative to the total file duration.
 * 
 * @see StyledTextEditor
 * @see TimeUtils
 * @see PlaybackTimerManager
 * @see SessionState
 */
class PlaybackTextPresenter : public juce::TextEditor::Listener,
                              public juce::MouseListener,
                              public SessionState::Listener,
                              public PlaybackTimerManager::Listener {
  public:
    /**
     * @brief Constructs the presenter and registers it as a listener to state changes.
     * @param ownerPanel Reference to the parent View container.
     */
    explicit PlaybackTextPresenter(ControlPanel &ownerPanel);

    /**
     * @brief Unregisters listeners and ensures clean shutdown.
     */
    ~PlaybackTextPresenter() override;

    /**
     * @brief Configures the initial visual properties of the time editors.
     * @details Sets default text, alignment, and read-only states based on 
     *          the current application configuration.
     */
    void initialiseEditors();

    /**
     * @brief Forced synchronization of the UI editors with the current state.
     * @details Retrieves the current playhead position from `AudioPlayer` and 
     *          formats it using `TimeUtils` before updating the text fields.
     */
    void updateEditors();

    /**
     * @brief Callback from the playback timer.
     * @details Implements a "Tick Throttling" mechanism where the UI is only 
     *          refreshed every N ticks to reduce CPU overhead on the Message Thread.
     */
    void playbackTimerTick() override {
        if (++tickCounter % 4 == 0) {
            updateEditors();
        }
    }

    /**
     * @brief Reacts to file changes by resetting the time display.
     * @param filePath The path to the new audio asset.
     */
    void fileChanged(const juce::String &filePath) override {
        juce::ignoreUnused(filePath);
        updateEditors();
    }
    
    /** @brief Required SessionState::Listener overrides. */
    void cutInChanged(double) override {}
    /** @brief Required SessionState::Listener overrides. */
    void cutOutChanged(double) override {}
    /** @brief Required SessionState::Listener overrides. */
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override {}

  private:
    /** @name TextEditor Implementation
     *  Logic for handling manual user input in the time fields.
     */
    /**@{*/
    void textEditorTextChanged(juce::TextEditor &editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor &editor) override;
    void textEditorFocusLost(juce::TextEditor &editor) override;
    /**@}*/

    /** @name Mouse Interaction
     *  Logic for time scrubbing and segment highlighting.
     */
    /**@{*/
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    /**@}*/

    /**
     * @brief Parses the editor's text and applies it to the Audio Engine.
     * @param editor The specific editor being committed.
     */
    void applyTimeEdit(juce::TextEditor &editor);

    /**
     * @brief Internal helper to update a field with a formatted time.
     * @param editor The field to update.
     * @param positionSeconds The raw time value.
     * @param isRemaining True if formatting as a countdown (negative prefix).
     */
    void syncEditorToPosition(juce::TextEditor &editor, double positionSeconds,
                              bool isRemaining = false);

    ControlPanel &owner;                    /**< Reference to the host layout container. */
    juce::String totalTimeStaticStr;        /**< Cached string of the total duration for display. */

    bool isEditingElapsed{false};           /**< Flag to prevent auto-sync while user is typing. */
    bool isEditingRemaining{false};         /**< Flag to prevent auto-sync while user is typing. */
    int tickCounter{0};                     /**< Throttling counter for UI refresh. */
};

#endif
