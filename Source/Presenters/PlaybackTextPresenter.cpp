

#include "Presenters/PlaybackTextPresenter.h"
#include "Utils/TimeUtils.h"

#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Utils/TimeEntryHelpers.h"
#include "Utils/TimeUtils.h"
#include <cmath>

PlaybackTextPresenter::PlaybackTextPresenter(ControlPanel &ownerPanel)

    : owner(ownerPanel) {
    owner.getSessionState().addListener(this);
}

PlaybackTextPresenter::~PlaybackTextPresenter() {
    owner.getSessionState().removeListener(this);
    owner.getPlaybackTimerManager().removeListener(this);
    if (owner.playbackTimeView != nullptr) {
        owner.playbackTimeView->getElapsedEditor().removeListener(this);
        owner.playbackTimeView->getRemainingEditor().removeListener(this);
        owner.playbackTimeView->getCutLengthEditor().removeListener(this);
    }
}

void PlaybackTextPresenter::initialiseEditors() {
    if (owner.playbackTimeView == nullptr)
        return;

    auto &elapsed = owner.playbackTimeView->getElapsedEditor();
    auto &remaining = owner.playbackTimeView->getRemainingEditor();
    auto &length = owner.playbackTimeView->getCutLengthEditor();

    elapsed.addListener(this);
    elapsed.addMouseListener(this, false);

    remaining.addListener(this);
    remaining.addMouseListener(this, false);

    length.addListener(this);
    length.addMouseListener(this, false);
}

void PlaybackTextPresenter::updateEditors() {
    if (owner.playbackTimeView == nullptr)
        return;

    auto &elapsed = owner.playbackTimeView->getElapsedEditor();
    auto &remaining = owner.playbackTimeView->getRemainingEditor();
    auto &length = owner.playbackTimeView->getCutLengthEditor();

    if (!isEditingElapsed && !elapsed.hasKeyboardFocus(true))
        syncEditorToPosition(elapsed, owner.getAudioPlayer().getCurrentPosition());

    if (!isEditingRemaining && !remaining.hasKeyboardFocus(true)) {
        const auto &session = owner.getSessionState();
        const auto cutOut = owner.getAudioPlayer().getCutOut();
        const auto remSeconds =
            juce::jmax(0.0, (session.getCutPrefs().active
                                 ? cutOut
                                 : owner.getAudioPlayer().getThumbnail().getTotalLength()) -
                                owner.getAudioPlayer().getCurrentPosition());

        syncEditorToPosition(remaining, remSeconds, true);
    }

    if (!isEditingCutLength && !length.hasKeyboardFocus(true)) {
        double cutLen = std::abs(owner.getSessionState().getCutOut() - owner.getSessionState().getCutIn());
        juce::String newText = TimeUtils::formatTime(cutLen);
        if (length.getText() != newText)
            length.setText(newText, juce::dontSendNotification);
    }
}

void PlaybackTextPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = true;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = true;
    else if (&editor == &owner.playbackTimeView->getCutLengthEditor())
        isEditingCutLength = true;

    const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

    TimeEntryHelpers::validateTimeEntry(editor, totalLength);
}

void PlaybackTextPresenter::textEditorReturnKeyPressed(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;
    else if (&editor == &owner.playbackTimeView->getCutLengthEditor())
        isEditingCutLength = false;

    applyTimeEdit(editor);
    editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorEscapeKeyPressed(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;
    else if (&editor == &owner.playbackTimeView->getCutLengthEditor())
        isEditingCutLength = false;

    updateEditors();
    editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorFocusLost(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;
    else if (&editor == &owner.playbackTimeView->getCutLengthEditor())
        isEditingCutLength = false;

    applyTimeEdit(editor);
}

void PlaybackTextPresenter::applyTimeEdit(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    double newTime = TimeUtils::parseTime(editor.getText());
    if (newTime < 0.0)
        return;

    double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    double cutOut = owner.getAudioPlayer().getCutOut();

    if (&editor == &owner.playbackTimeView->getElapsedEditor()) {
        owner.getAudioPlayer().setPlayheadPosition(newTime);
    } else if (&editor == &owner.playbackTimeView->getRemainingEditor()) {
        const auto &session = owner.getSessionState();
        const double base = session.getCutPrefs().active ? cutOut : totalLength;
        owner.getAudioPlayer().setPlayheadPosition(base - newTime);
    } else if (&editor == &owner.playbackTimeView->getCutLengthEditor()) {
        double currentIn = owner.getSessionState().getCutIn();
        newTime = juce::jlimit(0.0, totalLength, newTime);

        double proposedOut = currentIn + newTime;

        if (proposedOut > totalLength) {
            double newIn = totalLength - newTime;
            owner.getSessionState().setCutIn(newIn);
            owner.getSessionState().setCutOut(totalLength);
        } else {
            owner.getSessionState().setCutOut(proposedOut);
        }

        owner.getBoundaryLogicPresenter().ensureCutOrder();
        owner.getBoundaryLogicPresenter().refreshLabels();
    }

    updateEditors();
}

void PlaybackTextPresenter::syncEditorToPosition(juce::TextEditor &editor, double positionSeconds,
                                                 bool isRemaining) {
    if (owner.playbackTimeView == nullptr) return;
    if (editor.hasKeyboardFocus(true) ||
        (&editor == &owner.playbackTimeView->getElapsedEditor() && isEditingElapsed) ||
        (&editor == &owner.playbackTimeView->getRemainingEditor() && isEditingRemaining) ||
        (&editor == &owner.playbackTimeView->getCutLengthEditor() && isEditingCutLength)) {
        return;
    }

    juce::String text = TimeUtils::formatTime(positionSeconds);
    if (isRemaining)
        text = "-" + text;

    if (editor.getText() != text)
        editor.setText(text, juce::dontSendNotification);
}

void PlaybackTextPresenter::mouseDown(const juce::MouseEvent &event) {
    if (owner.playbackTimeView == nullptr) return;
    if (auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent)) {
        if (editor == &owner.playbackTimeView->getElapsedEditor())
            isEditingElapsed = true;
        else if (editor == &owner.playbackTimeView->getRemainingEditor())
            isEditingRemaining = true;
        else if (editor == &owner.playbackTimeView->getCutLengthEditor())
            isEditingCutLength = true;
    }
}

void PlaybackTextPresenter::mouseUp(const juce::MouseEvent &event) {
    if (owner.playbackTimeView == nullptr) return;
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return;

    if (editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = true;
    else if (editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = true;
    else if (editor == &owner.playbackTimeView->getCutLengthEditor())
        isEditingCutLength = true;

    TimeEntryHelpers::handleTimeSegmentHighlight(event);
}

void PlaybackTextPresenter::mouseMove(const juce::MouseEvent &event) {
    owner.getSessionState().setZoomFactor(TimeEntryHelpers::getZoomFactorForPosition(event));
}

void PlaybackTextPresenter::mouseWheelMove(const juce::MouseEvent &event,
                                           const juce::MouseWheelDetails &wheel) {
    if (wheel.deltaY == 0.0f)
        return;

    if (owner.playbackTimeView == nullptr) return;
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return;

    if (editor->hasKeyboardFocus(true) ||
        (editor == &owner.playbackTimeView->getElapsedEditor() && isEditingElapsed) ||
        (editor == &owner.playbackTimeView->getRemainingEditor() && isEditingRemaining) ||
        (editor == &owner.playbackTimeView->getCutLengthEditor() && isEditingCutLength))
        return;

    double currentVal = TimeUtils::parseTime(editor->getText());
    if (currentVal < 0.0)
        currentVal = 0.0;

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);

    double newVal = TimeEntryHelpers::handleTimeStep(event, wheel, currentVal, sampleRate);
    newVal = juce::jmax(0.0, newVal);

    if (editor == &owner.playbackTimeView->getElapsedEditor()) {
        owner.getAudioPlayer().setPlayheadPosition(newVal);
    } else if (editor == &owner.playbackTimeView->getRemainingEditor()) {
        double total = owner.getAudioPlayer().getThumbnail().getTotalLength();
        owner.getAudioPlayer().setPlayheadPosition(total - newVal);
    } else if (editor == &owner.playbackTimeView->getCutLengthEditor()) {
        const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
        newVal = juce::jlimit(0.0, totalLength, newVal);
        double proposedOut = owner.getSessionState().getCutIn() + newVal;

        if (proposedOut > totalLength) {
            owner.getSessionState().setCutIn(totalLength - newVal);
            owner.getSessionState().setCutOut(totalLength);
        } else {
            owner.getSessionState().setCutOut(proposedOut);
        }

        owner.getBoundaryLogicPresenter().ensureCutOrder();
        owner.getBoundaryLogicPresenter().refreshLabels();
    }

    updateEditors();
}
