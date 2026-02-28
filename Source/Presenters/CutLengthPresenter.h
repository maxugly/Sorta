
#ifndef AUDIOFILER_CUTLENGTHPRESENTER_H
#define AUDIOFILER_CUTLENGTHPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

class ControlPanel;

class CutLengthPresenter : public juce::TextEditor::Listener,
                           public juce::MouseListener,
                           public SessionState::Listener {
public:
    explicit CutLengthPresenter(ControlPanel& ownerPanel);
    ~CutLengthPresenter() override;

    void initialiseEditors();
    void updateEditor();

    // SessionState::Listener
    void fileChanged(const juce::String& filePath) override {
        juce::ignoreUnused(filePath);
        updateEditor();
    }
    void cutInChanged(double) override { updateEditor(); }
    void cutOutChanged(double) override { updateEditor(); }
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override { updateEditor(); }

private:
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;

    void mouseWheelMove(const juce::MouseEvent& event,
                        const juce::MouseWheelDetails& wheel) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void applyTimeEdit(juce::TextEditor& editor);

    ControlPanel& owner;
    bool isEditingCutLength{ false };
};

#endif
