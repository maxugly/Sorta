#include "Presenters/BoundaryLogicPresenter.h"
#include "Core/AudioPlayer.h"
#include "Presenters/CutButtonPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/FocusManager.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"
#include "Utils/PlaybackHelpers.h"
#include "Utils/TimeEntryHelpers.h"
#include "Utils/TimeUtils.h"
#include "Workers/SilenceDetector.h"

BoundaryLogicPresenter::BoundaryLogicPresenter(ControlPanel &ownerPanel, SilenceDetector &detector,
                                               juce::TextEditor &cutIn, juce::TextEditor &cutOut)
    : owner(ownerPanel), silenceDetector(detector), cutInEditor(cutIn), cutOutEditor(cutOut) {
    cutInEditor.addListener(this);
    cutOutEditor.addListener(this);
    cutInEditor.addMouseListener(this, false);
    cutOutEditor.addMouseListener(this, false);
}

BoundaryLogicPresenter::~BoundaryLogicPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
    cutInEditor.removeListener(this);
    cutOutEditor.removeListener(this);
    cutInEditor.removeMouseListener(this);
    cutOutEditor.removeMouseListener(this);
}

void BoundaryLogicPresenter::initialiseEditors() {
    auto configure = [](juce::TextEditor &editor) {
        editor.setReadOnly(false);
        editor.setJustification(juce::Justification::centred);
        editor.setColour(juce::TextEditor::backgroundColourId,
                         Config::Colors::textEditorBackground);
        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        editor.setFont(juce::Font(juce::FontOptions(Config::Layout::Text::playbackSize)));
        editor.setMultiLine(false);
        editor.setReturnKeyStartsNewLine(false);
        editor.setWantsKeyboardFocus(true);
        editor.setSelectAllWhenFocused(true);
    };

    configure(cutInEditor);
    configure(cutOutEditor);
}

void BoundaryLogicPresenter::refreshLabels() {
    const double currentIn = owner.getAudioPlayer().getCutIn();
    const double currentOut = owner.getAudioPlayer().getCutOut();

    if (!isEditingIn && !cutInEditor.hasKeyboardFocus(true))
        syncEditorToPosition(cutInEditor, currentIn);

    if (!isEditingOut && !cutOutEditor.hasKeyboardFocus(true))
        syncEditorToPosition(cutOutEditor, currentOut);
}

void BoundaryLogicPresenter::ensureCutOrder() {
    auto &audioPlayer = owner.getAudioPlayer();
    double currentIn = audioPlayer.getCutIn();
    double currentOut = audioPlayer.getCutOut();

    if (currentIn > currentOut) {
        std::swap(currentIn, currentOut);
        audioPlayer.setCutIn(currentIn);
        audioPlayer.setCutOut(currentOut);

        bool acIn = silenceDetector.getIsAutoCutInActive();
        bool acOut = silenceDetector.getIsAutoCutOutActive();
        owner.getSessionState().setAutoCutInActive(acOut);
        owner.getSessionState().setAutoCutOutActive(acIn);
    }
}

void BoundaryLogicPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    if (&editor == &cutInEditor)
        isEditingIn = true;
    else if (&editor == &cutOutEditor)
        isEditingOut = true;

    TimeEntryHelpers::validateTimeEntry(editor, getAudioTotalLength());
}

void BoundaryLogicPresenter::textEditorReturnKeyPressed(juce::TextEditor &editor) {
    if (&editor == &cutInEditor)
        isEditingIn = false;
    if (&editor == &cutOutEditor)
        isEditingOut = false;

    const double newPosition = TimeUtils::parseTime(editor.getText());
    if (&editor == &cutInEditor)
        applyCutInFromEditor(newPosition, editor);
    else if (&editor == &cutOutEditor)
        applyCutOutFromEditor(newPosition, editor);

    editor.giveAwayKeyboardFocus();
}

void BoundaryLogicPresenter::textEditorEscapeKeyPressed(juce::TextEditor &editor) {
    if (&editor == &cutInEditor)
        isEditingIn = false;
    if (&editor == &cutOutEditor)
        isEditingOut = false;

    if (&editor == &cutInEditor)
        syncEditorToPosition(editor, owner.getAudioPlayer().getCutIn());
    else if (&editor == &cutOutEditor)
        syncEditorToPosition(editor, owner.getAudioPlayer().getCutOut());

    editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    editor.giveAwayKeyboardFocus();
}

void BoundaryLogicPresenter::textEditorFocusLost(juce::TextEditor &editor) {
    if (&editor == &cutInEditor)
        isEditingIn = false;
    if (&editor == &cutOutEditor)
        isEditingOut = false;

    const double newPosition = TimeUtils::parseTime(editor.getText());
    if (&editor == &cutInEditor)
        applyCutInFromEditor(newPosition, editor);
    else if (&editor == &cutOutEditor)
        applyCutOutFromEditor(newPosition, editor);

    owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::None);
}

void BoundaryLogicPresenter::mouseDown(const juce::MouseEvent &event) {
    if (event.eventComponent == &cutInEditor)
        isEditingIn = true;
    else if (event.eventComponent == &cutOutEditor)
        isEditingOut = true;
}

void BoundaryLogicPresenter::mouseUp(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor == nullptr)
        return;

    if (editor == &cutInEditor)
        isEditingIn = true;
    else if (editor == &cutOutEditor)
        isEditingOut = true;

    TimeEntryHelpers::handleTimeSegmentHighlight(event);
}

void BoundaryLogicPresenter::mouseEnter(const juce::MouseEvent &event) {
    if (event.eventComponent == &cutInEditor)
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::In);
    else if (event.eventComponent == &cutOutEditor)
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::Out);
}

void BoundaryLogicPresenter::mouseExit(const juce::MouseEvent &event) {
    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor != nullptr && !editor->hasKeyboardFocus(false))
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::None);
}

void BoundaryLogicPresenter::mouseWheelMove(const juce::MouseEvent &event,
                                            const juce::MouseWheelDetails &wheel) {
    if (wheel.deltaY == 0.0f)
        return;

    auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
    if (editor != nullptr) {
        if (editor->hasKeyboardFocus(true) || (editor == &cutInEditor && isEditingIn) ||
            (editor == &cutOutEditor && isEditingOut))
            return;
    }

    if (event.mods.isCtrlDown() && !event.mods.isShiftDown()) {
        owner.getSessionState().setZoomFactor(owner.getSessionState().getZoomFactor() * (wheel.deltaY > 0 ? 1.1f : 0.9f));
        return;
    }

    if (editor == nullptr)
        return;

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);

    if (editor == &cutInEditor) {
        const double currentIn = owner.getAudioPlayer().getCutIn();
        const double newIn = TimeEntryHelpers::handleTimeStep(event, wheel, currentIn, sampleRate);
        setCutInPosition(newIn);
        owner.getSessionState().setAutoCutInActive(false);
        owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);
    } else if (editor == &cutOutEditor) {
        const double currentOut = owner.getAudioPlayer().getCutOut();
        const double newOut = TimeEntryHelpers::handleTimeStep(event, wheel, currentOut, sampleRate);
        setCutOutPosition(newOut);
        owner.getSessionState().setAutoCutOutActive(false);
        owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);
    }

    ensureCutOrder();
    refreshLabels();
    owner.repaint();
}

void BoundaryLogicPresenter::syncEditorToPosition(juce::TextEditor &editor,
                                                  double positionSeconds) {
    if (editor.hasKeyboardFocus(true) || (&editor == &cutInEditor && isEditingIn) ||
        (&editor == &cutOutEditor && isEditingOut))
        return;

    juce::String newText = TimeUtils::formatTime(positionSeconds);
    if (editor.getText() != newText)
        editor.setText(newText, juce::dontSendNotification);
}

bool BoundaryLogicPresenter::applyCutInFromEditor(double newPosition, juce::TextEditor &editor) {
    const double totalLength = getAudioTotalLength();
    if (newPosition >= 0.0 && newPosition <= totalLength) {
        setCutInPosition(newPosition);
        owner.getPresenterCore().getCutButtonPresenter().updateColours();
        owner.getSessionState().setAutoCutInActive(false);

        if (owner.getInteractionCoordinator().getActiveZoomPoint() !=
            AppEnums::ActiveZoomPoint::None)
            owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);

        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        refreshLabels();
        owner.repaint();
        return true;
    }

    syncEditorToPosition(editor, owner.getAudioPlayer().getCutIn());
    editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    owner.repaint();
    return false;
}

bool BoundaryLogicPresenter::applyCutOutFromEditor(double newPosition, juce::TextEditor &editor) {
    const double totalLength = getAudioTotalLength();
    if (newPosition >= 0.0 && newPosition <= totalLength) {
        if (owner.getAudioPlayer().isRepeating() &&
            owner.getAudioPlayer().getCurrentPosition() >= owner.getAudioPlayer().getCutOut())
            owner.getAudioPlayer().setPlayheadPosition(owner.getAudioPlayer().getCutIn());

        setCutOutPosition(newPosition);
        owner.getPresenterCore().getCutButtonPresenter().updateColours();
        owner.getSessionState().setAutoCutOutActive(false);

        if (owner.getInteractionCoordinator().getActiveZoomPoint() !=
            AppEnums::ActiveZoomPoint::None)
            owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);

        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        refreshLabels();
        owner.repaint();
        return true;
    }

    syncEditorToPosition(editor, owner.getAudioPlayer().getCutOut());
    editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    owner.repaint();
    return false;
}

void BoundaryLogicPresenter::setCutInPosition(double positionSeconds) {
    const double totalLength = getAudioTotalLength();
    auto &audioPlayer = owner.getAudioPlayer();
    const double currentOut = audioPlayer.getCutOut();

    double newPos = PlaybackHelpers::constrainPosition(positionSeconds, 0.0, totalLength);

    if (!silenceDetector.getIsAutoCutInActive() && newPos >= currentOut &&
        silenceDetector.getIsAutoCutOutActive())
        owner.getSessionState().setAutoCutOutActive(false);

    audioPlayer.setCutIn(newPos);

    if (silenceDetector.getIsAutoCutInActive() && newPos >= currentOut) {
        setCutOutPosition(totalLength);
    }

    audioPlayer.setPlayheadPosition(audioPlayer.getCurrentPosition());
    ensureCutOrder();
}

void BoundaryLogicPresenter::setCutOutPosition(double positionSeconds) {
    const double totalLength = getAudioTotalLength();
    auto &audioPlayer = owner.getAudioPlayer();
    const double currentIn = audioPlayer.getCutIn();

    double newPos = PlaybackHelpers::constrainPosition(positionSeconds, 0.0, totalLength);

    if (!silenceDetector.getIsAutoCutOutActive() && newPos <= currentIn &&
        silenceDetector.getIsAutoCutInActive())
        owner.getSessionState().setAutoCutInActive(false);

    audioPlayer.setCutOut(newPos);

    if (silenceDetector.getIsAutoCutOutActive() && newPos <= currentIn) {
        setCutInPosition(0.0);
    }

    audioPlayer.setPlayheadPosition(audioPlayer.getCurrentPosition());
    ensureCutOrder();
}

double BoundaryLogicPresenter::getAudioTotalLength() const {
    return owner.getAudioPlayer().getThumbnail().getTotalLength();
}
