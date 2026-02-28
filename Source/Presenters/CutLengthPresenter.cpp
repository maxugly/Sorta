
#include "Presenters/CutLengthPresenter.h"
#include "Utils/TimeUtils.h"
#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Utils/TimeEntryHelpers.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include <cmath>

CutLengthPresenter::CutLengthPresenter(ControlPanel& ownerPanel)
    : owner(ownerPanel) {
    owner.getSessionState().addListener(this);
}

CutLengthPresenter::~CutLengthPresenter() {
    owner.getSessionState().removeListener(this);
    if (owner.getCutLengthStrip() != nullptr) {
        owner.getCutLengthStrip()->getLengthEditor().removeListener(this);
    }
}

void CutLengthPresenter::initialiseEditors() {
    if (owner.getCutLengthStrip() == nullptr)
        return;

    auto& length = owner.getCutLengthStrip()->getLengthEditor();
    length.addListener(this);
    length.addMouseListener(this, false);

    updateEditor();
}

void CutLengthPresenter::updateEditor() {
    if (owner.getCutLengthStrip() == nullptr)
        return;

    double sampleRate = 0.0;
    juce::int64 dummyLength = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, dummyLength);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    auto& length = owner.getCutLengthStrip()->getLengthEditor();
    if (!isEditingCutLength && !length.hasKeyboardFocus(true)) {
        double cutLen = std::abs(owner.getSessionState().getCutOut() - owner.getSessionState().getCutIn());
        juce::String newText = "+" + TimeUtils::formatTime(cutLen, sampleRate);
        if (length.getText() != newText)
            length.setText(newText, juce::dontSendNotification);
    }
}

void CutLengthPresenter::textEditorTextChanged(juce::TextEditor& editor) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (&editor == &owner.getCutLengthStrip()->getLengthEditor())
        isEditingCutLength = true;

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    TimeEntryHelpers::validateTimeEntry(editor, totalLength, sampleRate);
}

void CutLengthPresenter::textEditorReturnKeyPressed(juce::TextEditor& editor) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (&editor == &owner.getCutLengthStrip()->getLengthEditor())
        isEditingCutLength = false;

    applyTimeEdit(editor);
    editor.giveAwayKeyboardFocus();
}

void CutLengthPresenter::textEditorEscapeKeyPressed(juce::TextEditor& editor) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (&editor == &owner.getCutLengthStrip()->getLengthEditor())
        isEditingCutLength = false;

    updateEditor();
    editor.giveAwayKeyboardFocus();
}

void CutLengthPresenter::textEditorFocusLost(juce::TextEditor& editor) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (&editor == &owner.getCutLengthStrip()->getLengthEditor())
        isEditingCutLength = false;

    applyTimeEdit(editor);
}

void CutLengthPresenter::applyTimeEdit(juce::TextEditor& editor) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (&editor != &owner.getCutLengthStrip()->getLengthEditor()) return;

    double sampleRate = 0.0;
    juce::int64 dummyLength = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, dummyLength);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    double newTime = TimeUtils::parseTime(editor.getText(), sampleRate);
    if (newTime < 0.0)
        return;

    const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    const auto& prefs = owner.getSessionState().getCutPrefs();
    newTime = juce::jlimit(0.0, totalLength, newTime);

    if (prefs.outLocked && !prefs.inLocked) {
        owner.getSessionState().setCutIn(prefs.cutOut - newTime);
    } else {
        double proposedOut = prefs.cutIn + newTime;
        if (proposedOut > totalLength) {
            owner.getSessionState().setCutIn(totalLength - newTime);
            owner.getSessionState().setCutOut(totalLength);
        } else {
            owner.getSessionState().setCutOut(proposedOut);
        }
    }

    owner.getBoundaryLogicPresenter().ensureCutOrder();
    owner.getBoundaryLogicPresenter().refreshLabels();
    updateEditor();
}

void CutLengthPresenter::mouseDown(const juce::MouseEvent& event) {
    if (owner.getCutLengthStrip() == nullptr) return;
    if (auto* editor = dynamic_cast<juce::TextEditor*>(event.eventComponent)) {
        if (editor == &owner.getCutLengthStrip()->getLengthEditor())
            isEditingCutLength = true;
    }
}

void CutLengthPresenter::mouseUp(const juce::MouseEvent& event) {
    if (owner.getCutLengthStrip() == nullptr) return;
    auto* editor = dynamic_cast<juce::TextEditor*>(event.eventComponent);
    if (editor == nullptr)
        return;

    if (editor == &owner.getCutLengthStrip()->getLengthEditor())
        isEditingCutLength = true;

    TimeEntryHelpers::handleTimeSegmentHighlight(event);
}

void CutLengthPresenter::mouseMove(const juce::MouseEvent& event) {
    owner.getSessionState().setZoomFactor(TimeEntryHelpers::getZoomFactorForPosition(event));
}

void CutLengthPresenter::mouseWheelMove(const juce::MouseEvent& event,
                                        const juce::MouseWheelDetails& wheel) {
    if (wheel.deltaY == 0.0f)
        return;

    if (owner.getCutLengthStrip() == nullptr) return;
    auto* editor = dynamic_cast<juce::TextEditor*>(event.eventComponent);
    if (editor == nullptr || editor != &owner.getCutLengthStrip()->getLengthEditor())
        return;

    if (editor->hasKeyboardFocus(true) || isEditingCutLength)
        return;

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    double currentVal = TimeUtils::parseTime(editor->getText(), sampleRate);
    if (currentVal < 0.0)
        currentVal = 0.0;

    double newVal = TimeEntryHelpers::handleTimeStep(event, wheel, currentVal, sampleRate);
    newVal = juce::jmax(0.0, newVal);

    const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    const auto& prefs = owner.getSessionState().getCutPrefs();
    newVal = juce::jlimit(0.0, totalLength, newVal);

    if (prefs.outLocked && !prefs.inLocked) {
        owner.getSessionState().setCutIn(prefs.cutOut - newVal);
    } else {
        double proposedOut = prefs.cutIn + newVal;
        if (proposedOut > totalLength) {
            owner.getSessionState().setCutIn(totalLength - newVal);
            owner.getSessionState().setCutOut(totalLength);
        } else {
            owner.getSessionState().setCutOut(proposedOut);
        }
    }

    owner.getBoundaryLogicPresenter().ensureCutOrder();
    owner.getBoundaryLogicPresenter().refreshLabels();
    updateEditor();
}
