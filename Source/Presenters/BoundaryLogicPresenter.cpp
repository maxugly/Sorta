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

BoundaryLogicPresenter::BoundaryLogicPresenter(ControlPanel &ownerPanel,
                                               juce::TextEditor &cutIn, juce::TextEditor &cutOut)
    : owner(ownerPanel), cutInEditor(cutIn), cutOutEditor(cutOut) {
    cutInEditor.addListener(this);
    cutOutEditor.addListener(this);
    cutInEditor.addMouseListener(this, false);
    cutOutEditor.addMouseListener(this, false);
    owner.getSessionState().addListener(this);
}

BoundaryLogicPresenter::~BoundaryLogicPresenter() {
    owner.getSessionState().removeListener(this);
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

        bool acIn = owner.getSessionState().getCutPrefs().autoCut.inActive;
        bool acOut = owner.getSessionState().getCutPrefs().autoCut.outActive;
        owner.getSessionState().setAutoCutInActive(acOut);
        owner.getSessionState().setAutoCutOutActive(acIn);
    }
}

void BoundaryLogicPresenter::textEditorTextChanged(juce::TextEditor &editor) {
    if (&editor == &cutInEditor) {
        isEditingIn = true;
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::In);
    } else if (&editor == &cutOutEditor) {
        isEditingOut = true;
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::Out);
    }

    TimeEntryHelpers::validateTimeEntry(editor, getAudioTotalLength());
}

void BoundaryLogicPresenter::textEditorReturnKeyPressed(juce::TextEditor &editor) {
    if (&editor == &cutInEditor) isEditingIn = false;
    if (&editor == &cutOutEditor) isEditingOut = false;

    applyCutFromEditor(editor, TimeUtils::parseTime(editor.getText()));
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
    if (&editor == &cutInEditor) isEditingIn = false;
    if (&editor == &cutOutEditor) isEditingOut = false;

    applyCutFromEditor(editor, TimeUtils::parseTime(editor.getText()));
    owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::None);
}

void BoundaryLogicPresenter::mouseDown(const juce::MouseEvent &event) {
    if (event.eventComponent == &cutInEditor) {
        isEditingIn = true;
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::In);
    } else if (event.eventComponent == &cutOutEditor) {
        isEditingOut = true;
        owner.getInteractionCoordinator().setManualZoomPoint(AppEnums::ActiveZoomPoint::Out);
    }
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

void BoundaryLogicPresenter::mouseMove(const juce::MouseEvent &event) {
    owner.getSessionState().setZoomFactor(TimeEntryHelpers::getZoomFactorForPosition(event));
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

    const bool isIn = (editor == &cutInEditor);
    owner.getInteractionCoordinator().setManualZoomPoint(isIn ? AppEnums::ActiveZoomPoint::In : AppEnums::ActiveZoomPoint::Out);
    const double currentPos = isIn ? owner.getAudioPlayer().getCutIn() : owner.getAudioPlayer().getCutOut();
    const double newPos = TimeEntryHelpers::handleTimeStep(event, wheel, currentPos, sampleRate);

    setCutPosition(isIn, newPos);
    if (isIn) owner.getSessionState().setAutoCutInActive(false);
    else owner.getSessionState().setAutoCutOutActive(false);

    owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);

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

bool BoundaryLogicPresenter::applyCutFromEditor(juce::TextEditor &editor, double newPosition) {
    const double totalLength = getAudioTotalLength();
    const bool isIn = (&editor == &cutInEditor);

    if (newPosition >= 0.0 && newPosition <= totalLength) {
        if (!isIn && owner.getAudioPlayer().isRepeating() &&
            owner.getAudioPlayer().getCurrentPosition() >= owner.getAudioPlayer().getCutOut())
            owner.getAudioPlayer().setPlayheadPosition(owner.getAudioPlayer().getCutIn());

        setCutPosition(isIn, newPosition);
        if (isIn) owner.getSessionState().setAutoCutInActive(false);
        else owner.getSessionState().setAutoCutOutActive(false);

        if (owner.getInteractionCoordinator().getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None)
            owner.getInteractionCoordinator().setNeedsJumpToCutIn(true);

        editor.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
        refreshLabels();
        owner.repaint();
        return true;
    }

    syncEditorToPosition(editor, isIn ? owner.getAudioPlayer().getCutIn() : owner.getAudioPlayer().getCutOut());
    editor.setColour(juce::TextEditor::textColourId, Config::Colors::textEditorError);
    owner.repaint();
    return false;
}

void BoundaryLogicPresenter::setCutPosition(bool isIn, double positionSeconds) {
    const double totalLength = getAudioTotalLength();
    auto &audioPlayer = owner.getAudioPlayer();
    const double otherPos = isIn ? audioPlayer.getCutOut() : audioPlayer.getCutIn();

    double newPos = PlaybackHelpers::constrainPosition(positionSeconds, 0.0, totalLength);

    if (isIn) {
        if (!owner.getSessionState().getCutPrefs().autoCut.inActive && newPos >= otherPos &&
            owner.getSessionState().getCutPrefs().autoCut.outActive)
            owner.getSessionState().setAutoCutOutActive(false);

        audioPlayer.setCutIn(newPos);
        if (owner.getSessionState().getCutPrefs().autoCut.inActive && newPos >= otherPos)
            setCutPosition(false, totalLength);
    } else {
        if (!owner.getSessionState().getCutPrefs().autoCut.outActive && newPos <= otherPos &&
            owner.getSessionState().getCutPrefs().autoCut.inActive)
            owner.getSessionState().setAutoCutInActive(false);

        audioPlayer.setCutOut(newPos);
        if (owner.getSessionState().getCutPrefs().autoCut.outActive && newPos <= otherPos)
            setCutPosition(true, 0.0);
    }

    audioPlayer.setPlayheadPosition(audioPlayer.getCurrentPosition());
    ensureCutOrder();
}

double BoundaryLogicPresenter::getAudioTotalLength() const {
    return owner.getAudioPlayer().getThumbnail().getTotalLength();
}
