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
}

SilenceThresholdPresenter::~SilenceThresholdPresenter() {
    inThresholdEditor.removeListener(this);
    outThresholdEditor.removeListener(this);
    inThresholdEditor.removeMouseListener(this);
    outThresholdEditor.removeMouseListener(this);
}

void SilenceThresholdPresenter::configureEditor(juce::TextEditor &editor, float initialValue,
                                               const juce::String &tooltip) {
    editor.setReadOnly(false);
    editor.setJustification(juce::Justification::centred);
    editor.setFont(juce::Font(juce::FontOptions(Config::Layout::Text::playbackSize * 0.8f)));
    editor.setInputRestrictions(2, "0123456789");
    editor.setTooltip(tooltip);
    editor.setText(juce::String(juce::roundToInt(initialValue * 100.0f)), juce::dontSendNotification);

    editor.addListener(this);
    editor.addMouseListener(this, false);
}

void SilenceThresholdPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    const int val = editor.getText().getIntValue();
    if (!isValidPercentage(val)) {
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    } else {
        // If it was error, restore it to custom color (when focused)
        if (editor.getProperties().contains("CustomTextColor")) {
             auto textColor = juce::Colour::fromString(editor.getProperties()["CustomTextColor"].toString());
             editor.setColour(juce::TextEditor::textColourId, textColor);
        }
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
        if (&editor == &inThresholdEditor)
            owner.getSessionState().setThresholdIn(threshold);
        else
            owner.getSessionState().setThresholdOut(threshold);
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
}
