#pragma once

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class LoopEditorPresenter
 * @brief Configures and validates the loop start/end editors for ControlPanel.
 */
class LoopEditorPresenter final : private juce::TextEditor::Listener
{
public:
    explicit LoopEditorPresenter(ControlPanel& ownerPanel);
    ~LoopEditorPresenter() override;

    void initialiseEditors();

private:
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;

    void applyLoopEdit(juce::TextEditor& editor, bool isLoopIn);
    void restoreEditorValue(juce::TextEditor& editor, bool isLoopIn);
    double parseTime(const juce::String& timeString) const;

    ControlPanel& owner;
    juce::TextEditor& loopInEditor;
    juce::TextEditor& loopOutEditor;
};

