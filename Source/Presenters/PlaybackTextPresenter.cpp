

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
    }
}

void PlaybackTextPresenter::initialiseEditors() {
    if (owner.playbackTimeView == nullptr)
        return;

    auto &elapsed = owner.playbackTimeView->getElapsedEditor();
    auto &remaining = owner.playbackTimeView->getRemainingEditor();

    elapsed.addListener(this);
    elapsed.addMouseListener(this, false);

    remaining.addListener(this);
    remaining.addMouseListener(this, false);
}

void PlaybackTextPresenter::updateEditors() {
    if (owner.playbackTimeView == nullptr)
        return;

    double sampleRate = 0.0;
    juce::int64 dummyLength = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, dummyLength);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    auto totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    owner.playbackTimeView->updateTimes(
        TimeUtils::formatTime(owner.getAudioPlayer().getCurrentPosition(), sampleRate),
        "+" + TimeUtils::formatTime(totalLength, sampleRate));

    auto &elapsed = owner.playbackTimeView->getElapsedEditor();
    auto &total = owner.playbackTimeView->getTotalTimeEditor();
    auto &remaining = owner.playbackTimeView->getRemainingEditor();

    juce::String totalTimeFormatted = "+" + TimeUtils::formatTime(totalLength, sampleRate);
    if (total.getText() != totalTimeFormatted)
        total.setText(totalTimeFormatted, juce::dontSendNotification);

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
}

void PlaybackTextPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = true;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = true;

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

    TimeEntryHelpers::validateTimeEntry(editor, totalLength, sampleRate);
}

void PlaybackTextPresenter::textEditorReturnKeyPressed(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;

    applyTimeEdit(editor);
    editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorEscapeKeyPressed(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;

    updateEditors();
    editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorFocusLost(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;
    if (&editor == &owner.playbackTimeView->getElapsedEditor())
        isEditingElapsed = false;
    else if (&editor == &owner.playbackTimeView->getRemainingEditor())
        isEditingRemaining = false;

    applyTimeEdit(editor);
}

void PlaybackTextPresenter::applyTimeEdit(juce::TextEditor &editor) {
    if (owner.playbackTimeView == nullptr) return;

    double sampleRate = 0.0;
    juce::int64 dummyLength = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, dummyLength);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    double newTime = TimeUtils::parseTime(editor.getText(), sampleRate);
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
    }

    updateEditors();
}

void PlaybackTextPresenter::syncEditorToPosition(juce::TextEditor &editor, double positionSeconds,
                                                 bool isRemaining) {
    if (owner.playbackTimeView == nullptr) return;
    if (editor.hasKeyboardFocus(true) ||
        (&editor == &owner.playbackTimeView->getElapsedEditor() && isEditingElapsed) ||
        (&editor == &owner.playbackTimeView->getRemainingEditor() && isEditingRemaining)) {
        return;
    }

    double sampleRate = 0.0;
    juce::int64 dummyLength = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, dummyLength);
    if (sampleRate <= 0) sampleRate = Config::Audio::fallbackSampleRate;

    juce::String text = TimeUtils::formatTime(positionSeconds, sampleRate);
    if (isRemaining)
        text = "-" + text;
    else
        text = "+" + text;

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

    TimeEntryHelpers::handleTimeSegmentHighlight(event);
}

void PlaybackTextPresenter::mouseMove(const juce::MouseEvent &event) {
    if (TimeEntryHelpers::shouldShowZoomPopup(event)) {
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::Playback);
        owner.getSessionState().setZoomFactor(TimeEntryHelpers::getZoomFactorForPosition(event));
    } else {
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::None);
    }
}

void PlaybackTextPresenter::mouseEnter(const juce::MouseEvent &event) {
    if (TimeEntryHelpers::shouldShowZoomPopup(event))
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::Playback);
}

void PlaybackTextPresenter::mouseExit(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor != nullptr && !editor->hasKeyboardFocus(false))
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::None);
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
        (editor == &owner.playbackTimeView->getRemainingEditor() && isEditingRemaining))
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

    if (editor == &owner.playbackTimeView->getElapsedEditor()) {
        owner.getAudioPlayer().setPlayheadPosition(newVal);
    } else if (editor == &owner.playbackTimeView->getRemainingEditor()) {
        double total = owner.getAudioPlayer().getThumbnail().getTotalLength();
        owner.getAudioPlayer().setPlayheadPosition(total - newVal);
    }

    updateEditors();
}
