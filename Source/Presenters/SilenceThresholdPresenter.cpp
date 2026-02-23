#include "Presenters/SilenceThresholdPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

SilenceThresholdPresenter::SilenceThresholdPresenter(ControlPanel &ownerPanel, 
                                                     juce::TextEditor &inEditor, 
                                                     juce::TextEditor &outEditor)
    : owner(ownerPanel), inThresholdEditor(inEditor), outThresholdEditor(outEditor) {
    
    const auto& autoCut = owner.getSessionState().getCutPrefs().autoCut;
    configureEditor(inThresholdEditor, autoCut.thresholdIn, "In Silence Threshold (%)");
    configureEditor(outThresholdEditor, autoCut.thresholdOut, "Out Silence Threshold (%)");

    startTimer(100);
}

SilenceThresholdPresenter::~SilenceThresholdPresenter() {
    inThresholdEditor.removeListener(this);
    outThresholdEditor.removeListener(this);
    inThresholdEditor.removeMouseListener(this);
    outThresholdEditor.removeMouseListener(this);
}

void SilenceThresholdPresenter::timerCallback() {
    if (pendingIn.active) {
        owner.getSessionState().setThresholdIn(pendingIn.value);
        pendingIn.active = false;
    }
    if (pendingOut.active) {
        owner.getSessionState().setThresholdOut(pendingOut.value);
        pendingOut.active = false;
    }
}

void SilenceThresholdPresenter::configureEditor(juce::TextEditor &editor, float initialValue,
                                               const juce::String &tooltip) {
    editor.setReadOnly(false);
    editor.setJustification(juce::Justification::centred);
    editor.setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    editor.setFont(juce::Font(juce::FontOptions(Config::Layout::Text::playbackSize * 0.8f)));
    editor.setInputRestrictions(2, "0123456789");
    editor.setTooltip(tooltip);
    editor.setText(juce::String(juce::roundToInt(initialValue * 100.0f)), juce::dontSendNotification);

    editor.addListener(this);
    editor.addMouseListener(this, false);
}

void SilenceThresholdPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    const int val = editor.getText().getIntValue();
    if (isValidPercentage(val)) {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    } else {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    }
}

void SilenceThresholdPresenter::textEditorReturnKeyPressed(juce::TextEditor &editor) {
    applyThresholdFromEditor(editor);
}

void SilenceThresholdPresenter::textEditorFocusLost(juce::TextEditor &editor) {
    applyThresholdFromEditor(editor);
}

void SilenceThresholdPresenter::mouseWheelMove(const juce::MouseEvent &event,
                                              const juce::MouseWheelDetails &wheel) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr || wheel.deltaY == 0.0f)
        return;

    int currentVal = editor->getText().getIntValue();
    int newVal = juce::jlimit(1, 99, currentVal + (wheel.deltaY > 0 ? 1 : -1));

    if (newVal != currentVal) {
        editor->setText(juce::String(newVal), juce::dontSendNotification);
        applyThresholdFromEditor(*editor);
    }
}

void SilenceThresholdPresenter::applyThresholdFromEditor(juce::TextEditor &editor) {
    const int val = editor.getText().getIntValue();
    if (isValidPercentage(val)) {
        const float threshold = static_cast<float>(val) / 100.0f;
        const bool isIn = (&editor == &inThresholdEditor);
        const float currentThreshold = isIn ? owner.getSessionState().getCutPrefs().autoCut.thresholdIn 
                                            : owner.getSessionState().getCutPrefs().autoCut.thresholdOut;

        if (std::abs(threshold - currentThreshold) > 0.001f) {
            if (isIn) {
                pendingIn.value = threshold;
                pendingIn.active = true;
            } else {
                pendingOut.value = threshold;
                pendingOut.active = true;
            }
        }
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    } else {
        restoreEditorToCurrentValue(editor);
    }
    editor.giveAwayKeyboardFocus();
}

void SilenceThresholdPresenter::updateThresholdFromEditorIfValid(juce::TextEditor &editor) {
    applyThresholdFromEditor(editor);
}

void SilenceThresholdPresenter::restoreEditorToCurrentValue(juce::TextEditor &editor) {
    const bool isIn = (&editor == &inThresholdEditor);
    const float currentThreshold = isIn ? owner.getSessionState().getCutPrefs().autoCut.thresholdIn 
                                        : owner.getSessionState().getCutPrefs().autoCut.thresholdOut;
    editor.setText(juce::String(juce::roundToInt(currentThreshold * 100.0f)), juce::dontSendNotification);
    editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
}
