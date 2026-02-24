

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

class PlaybackTextPresenter : public juce::TextEditor::Listener,
                              public juce::MouseListener,
                              public SessionState::Listener,
                              public PlaybackTimerManager::Listener {
  public:
    explicit PlaybackTextPresenter(ControlPanel &ownerPanel);

    ~PlaybackTextPresenter() override;

    void initialiseEditors();

    void updateEditors();

    // PlaybackTimerManager::Listener
    void playbackTimerTick() override {
        updateEditors();
    }
    void animationUpdate(float breathingPulse) override {
        juce::ignoreUnused(breathingPulse);
    }

    // SessionState::Listener
    void fileChanged(const juce::String &filePath) override {
        juce::ignoreUnused(filePath);
        updateEditors();
        updateLengthEditor();
    }
    void cutInChanged(double) override { updateLengthEditor(); }
    void cutOutChanged(double) override { updateLengthEditor(); }
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override { updateLengthEditor(); }

  private:
    void updateLengthEditor();
    void textEditorTextChanged(juce::TextEditor &editor) override;

    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;

    void textEditorEscapeKeyPressed(juce::TextEditor &editor) override;

    void textEditorFocusLost(juce::TextEditor &editor) override;

    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    void mouseUp(const juce::MouseEvent &event) override;

    void mouseMove(const juce::MouseEvent &event) override;

    void applyTimeEdit(juce::TextEditor &editor);
    void syncEditorToPosition(juce::TextEditor &editor, double positionSeconds,
                              bool isRemaining = false);

    ControlPanel &owner;
    juce::String totalTimeStaticStr;

    bool isEditingElapsed{false};
    bool isEditingRemaining{false};
    bool isEditingCutLength{false};

    void mouseDown(const juce::MouseEvent &event) override;
};

#endif
