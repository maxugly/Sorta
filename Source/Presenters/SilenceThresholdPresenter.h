

#ifndef AUDIOFILER_SILENCETHRESHOLDPRESENTER_H
#define AUDIOFILER_SILENCETHRESHOLDPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

class SilenceThresholdPresenter final : private juce::TextEditor::Listener,
                                        public juce::MouseListener,
                                        private juce::Timer {
  public:
    SilenceThresholdPresenter(ControlPanel &ownerPanel, juce::TextEditor &inEditor, juce::TextEditor &outEditor);

    ~SilenceThresholdPresenter() override;

  private:
    void timerCallback() override;

    void configureEditor(juce::TextEditor &editor, float initialValue, const juce::String &tooltip);

    void textEditorTextChanged(juce::TextEditor &editor) override;

    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;

    void textEditorFocusLost(juce::TextEditor &editor) override;

    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    void applyThresholdFromEditor(juce::TextEditor &editor);

    void updateThresholdFromEditorIfValid(juce::TextEditor &editor);

    void restoreEditorToCurrentValue(juce::TextEditor &editor);
    bool isValidPercentage(int value) const noexcept {
        return value >= 1 && value <= 99;
    }

    ControlPanel &owner;
    juce::TextEditor &inThresholdEditor;
    juce::TextEditor &outThresholdEditor;

    struct PendingThreshold {
        float value{0.0f};
        bool isIn{true};
        bool active{false};
    } pendingIn, pendingOut;
};

#endif
