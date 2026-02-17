#include "PlaybackTextPresenter.h"
#include "TimeUtils.h"

#include "AudioPlayer.h"
#include "Config.h"
#include "ControlPanel.h"
#include "TimeEntryHelpers.h"
#include "TimeUtils.h"
#include <cmath>

PlaybackTextPresenter::PlaybackTextPresenter(ControlPanel &ownerPanel)
    : owner(ownerPanel) {}

PlaybackTextPresenter::~PlaybackTextPresenter() {
  owner.elapsedTimeEditor.removeListener(this);
  owner.remainingTimeEditor.removeListener(this);
  owner.cutLengthEditor.removeListener(this);
}

void PlaybackTextPresenter::initialiseEditors() {
  auto configure = [&](juce::TextEditor &ed, juce::Justification just) {
    owner.addAndMakeVisible(ed);
    ed.setReadOnly(false);
    ed.setJustification(just);
    ed.setColour(juce::TextEditor::backgroundColourId,
                 juce::Colours::transparentBlack);
    ed.setColour(juce::TextEditor::outlineColourId,
                 juce::Colours::transparentBlack);
    ed.setColour(juce::TextEditor::textColourId, Config::Colors::playbackText);
    ed.setFont(juce::Font(
        juce::FontOptions((float)Config::Layout::Text::playbackSize)));
    ed.applyFontToAllText(ed.getFont());
    ed.setMultiLine(false);
    ed.setReturnKeyStartsNewLine(false);
    ed.setSelectAllWhenFocused(true);
    ed.addListener(this);
    ed.addMouseListener(this, false);
  };

  configure(owner.elapsedTimeEditor, juce::Justification::left);
  configure(owner.remainingTimeEditor, juce::Justification::right);
  configure(owner.cutLengthEditor, juce::Justification::centred);
}

void PlaybackTextPresenter::updateEditors() {
  const double currentPos = owner.getAudioPlayer().getTransportSource().getCurrentPosition();
  const double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

  double effectiveTotal = totalLength;
  double effectiveElapsed = currentPos;
  double effectiveRemaining = totalLength - currentPos;

  if (owner.isCutModeActive())
  {
      effectiveTotal = owner.getCutOutPosition() - owner.getCutInPosition();
      effectiveElapsed = currentPos - owner.getCutInPosition();
      effectiveRemaining = owner.getCutOutPosition() - currentPos;
  }

  // Clamp negative values (e.g. if playhead is before cutIn or after cutOut temporarily)
  // Although "effectiveElapsed" can be negative if we are before cutIn, which is useful info.
  // But standard display usually clamps or shows negative. Let's just pass raw values and see.
  // Actually, formatTime likely handles positive numbers nicely.

  if (!isEditingElapsed && !owner.elapsedTimeEditor.hasKeyboardFocus(true))
    syncEditorToPosition(owner.elapsedTimeEditor, juce::jmax(0.0, effectiveElapsed));

  if (!isEditingRemaining && !owner.remainingTimeEditor.hasKeyboardFocus(true)) {
    syncEditorToPosition(owner.remainingTimeEditor, juce::jmax(0.0, effectiveRemaining), true);
  }

  if (!isEditingLoopLength && !owner.cutLengthEditor.hasKeyboardFocus(true)) {
    // If cut mode is active, loop length IS the effective total duration.
    // If not, it's just the distance between markers.
    double length = std::abs(owner.getCutOutPosition() - owner.getCutInPosition());
    juce::String newText = owner.formatTime(length);
    if (owner.cutLengthEditor.getText() != newText)
      owner.cutLengthEditor.setText(newText, juce::dontSendNotification);
  }
}

void PlaybackTextPresenter::layoutEditors() {
  const int textY =
      owner.getBottomRowTopY() - Config::Layout::Text::playbackOffsetY;
  auto [leftX, centreX, rightX] = owner.getPlaybackLabelXs();

  owner.elapsedTimeEditor.setBounds(leftX, textY,
                                    Config::Layout::Text::playbackWidth,
                                    Config::Layout::Text::playbackHeight);
  owner.remainingTimeEditor.setBounds(rightX, textY,
                                      Config::Layout::Text::playbackWidth,
                                      Config::Layout::Text::playbackHeight);

  // The center is special because it's shared with total time which is static
  owner.cutLengthEditor.setBounds(centreX, textY,
                                   Config::Layout::Text::playbackWidth / 2,
                                   Config::Layout::Text::playbackHeight);
}

void PlaybackTextPresenter::render(juce::Graphics &g) const {
  if (owner.getAudioPlayer().getThumbnail().getTotalLength() <= 0.0)
    return;

  const int textY =
      owner.getBottomRowTopY() - Config::Layout::Text::playbackOffsetY;
  auto [leftX, centreX, rightX] = owner.getPlaybackLabelXs();

  g.setColour(Config::Colors::playbackText);
  g.setFont((float)Config::Layout::Text::playbackSize);

  // Calculate what "Total Time" to show
  double displayTotal = owner.getAudioPlayer().getThumbnail().getTotalLength();
  if (owner.isCutModeActive())
  {
      displayTotal = owner.getCutOutPosition() - owner.getCutInPosition();
  }

  juce::String totalTimeStr = " / " + owner.formatTime(displayTotal);

  g.drawText(totalTimeStr, centreX + (Config::Layout::Text::playbackWidth / 2),
             textY, Config::Layout::Text::playbackWidth / 2,
             Config::Layout::Text::playbackHeight, juce::Justification::left,
             false);
}

void PlaybackTextPresenter::textEditorTextChanged(juce::TextEditor &editor) {
  if (&editor == &owner.elapsedTimeEditor)
    isEditingElapsed = true;
  else if (&editor == &owner.remainingTimeEditor)
    isEditingRemaining = true;
  else if (&editor == &owner.cutLengthEditor)
    isEditingLoopLength = true;

  double validateMax = owner.getAudioPlayer().getThumbnail().getTotalLength();
  if (owner.isCutModeActive() && (&editor == &owner.elapsedTimeEditor || &editor == &owner.remainingTimeEditor))
  {
      validateMax = owner.getCutOutPosition() - owner.getCutInPosition();
  }

  TimeEntryHelpers::validateTimeEntry(editor, validateMax);
}

void PlaybackTextPresenter::textEditorReturnKeyPressed(
    juce::TextEditor &editor) {
  if (&editor == &owner.elapsedTimeEditor)
    isEditingElapsed = false;
  else if (&editor == &owner.remainingTimeEditor)
    isEditingRemaining = false;
  else if (&editor == &owner.cutLengthEditor)
    isEditingLoopLength = false;

  applyTimeEdit(editor);
  editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorEscapeKeyPressed(
    juce::TextEditor &editor) {
  if (&editor == &owner.elapsedTimeEditor)
    isEditingElapsed = false;
  else if (&editor == &owner.remainingTimeEditor)
    isEditingRemaining = false;
  else if (&editor == &owner.cutLengthEditor)
    isEditingLoopLength = false;

  updateEditors(); // Reset to current state
  editor.giveAwayKeyboardFocus();
}

void PlaybackTextPresenter::textEditorFocusLost(juce::TextEditor &editor) {
  if (&editor == &owner.elapsedTimeEditor)
    isEditingElapsed = false;
  else if (&editor == &owner.remainingTimeEditor)
    isEditingRemaining = false;
  else if (&editor == &owner.cutLengthEditor)
    isEditingLoopLength = false;

  applyTimeEdit(editor);
}


void PlaybackTextPresenter::applyTimeEdit(juce::TextEditor &editor) {
  double newTime = TimeUtils::parseTime(editor.getText());
  if (newTime < 0.0)
    return;

  auto &transport = owner.getAudioPlayer().getTransportSource();
  double totalLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

  if (&editor == &owner.elapsedTimeEditor) {
    double absolutePos = newTime;
    if (owner.isCutModeActive())
    {
        // Treat newTime as relative to cutIn
        absolutePos = owner.getCutInPosition() + newTime;
        // Constrain
        absolutePos = juce::jlimit(owner.getCutInPosition(), owner.getCutOutPosition(), absolutePos);
    }
    else
    {
        absolutePos = juce::jlimit(0.0, totalLength, absolutePos);
    }
    transport.setPosition(absolutePos);

  } else if (&editor == &owner.remainingTimeEditor) {
      double absolutePos;
      if (owner.isCutModeActive())
      {
          // remaining = (cutOut - pos) -> pos = cutOut - remaining
          absolutePos = owner.getCutOutPosition() - newTime;
          absolutePos = juce::jlimit(owner.getCutInPosition(), owner.getCutOutPosition(), absolutePos);
      }
      else
      {
          absolutePos = totalLength - newTime;
          absolutePos = juce::jlimit(0.0, totalLength, absolutePos);
      }
      transport.setPosition(absolutePos);

  } else if (&editor == &owner.cutLengthEditor) {
    // If cut mode is active, editing length changes cutOut
    double currentIn = owner.getCutInPosition();
    newTime = juce::jlimit(0.0, totalLength, newTime); // basic sanity check

    double proposedOut = currentIn + newTime;

    if (proposedOut > totalLength) {
      double newIn = totalLength - newTime;
      owner.setCutInPosition(newIn);
      owner.setCutOutPosition(totalLength);
    } else {
      owner.setCutOutPosition(proposedOut);
    }

    owner.ensureCutOrder();
    owner.updateCutLabels();
  }

  updateEditors();
}

void PlaybackTextPresenter::syncEditorToPosition(juce::TextEditor &editor,
                                                 double positionSeconds,
                                                 bool isRemaining) {
  // Guard against timer overwriting while editing OR focused
  if (editor.hasKeyboardFocus(true) ||
      (&editor == &owner.elapsedTimeEditor && isEditingElapsed) ||
      (&editor == &owner.remainingTimeEditor && isEditingRemaining) ||
      (&editor == &owner.cutLengthEditor && isEditingLoopLength)) {
    return;
  }

  juce::String text = owner.formatTime(positionSeconds);
  if (isRemaining)
    text = "-" + text;

  if (editor.getText() != text)
    editor.setText(text, juce::dontSendNotification);
}

void PlaybackTextPresenter::mouseDown(const juce::MouseEvent &event) {
  if (auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent)) {
    if (editor == &owner.elapsedTimeEditor)
      isEditingElapsed = true;
    else if (editor == &owner.remainingTimeEditor)
      isEditingRemaining = true;
    else if (editor == &owner.cutLengthEditor)
      isEditingLoopLength = true;
  }
}

void PlaybackTextPresenter::mouseUp(const juce::MouseEvent &event) {
  auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
  if (editor == nullptr)
    return;

  // Set flags to block timer. 
  // CRITICAL: Do NOT call grabKeyboardFocus here; let juce::TextEditor's internal logic 
  // handle the click/focus sequence to avoid selection reset bugs.
  if (editor == &owner.elapsedTimeEditor)
    isEditingElapsed = true;
  else if (editor == &owner.remainingTimeEditor)
    isEditingRemaining = true;
  else if (editor == &owner.cutLengthEditor)
    isEditingLoopLength = true;

  // Check if it's remaining time (starts with '-')
  bool isNegative = (editor == &owner.remainingTimeEditor) ||
                    editor->getText().startsWith("-");
  int offset = isNegative ? 1 : 0;

  int charIndex = editor->getTextIndexAt(event.getPosition());
  if (charIndex < 0)
    return;

  int effectiveIndex = charIndex - offset;

  // Time format: [opt -]HH:MM:SS:mmm
  juce::Range<int> newRange;

  if (effectiveIndex <= 1)
    newRange = juce::Range<int>(0 + offset, 2 + offset); // HH
  else if (effectiveIndex >= 3 && effectiveIndex <= 4)
    newRange = juce::Range<int>(3 + offset, 5 + offset); // MM
  else if (effectiveIndex >= 6 && effectiveIndex <= 7)
    newRange = juce::Range<int>(6 + offset, 8 + offset); // SS
  else if (effectiveIndex >= 9 && effectiveIndex <= 11)
    newRange = juce::Range<int>(9 + offset, 12 + offset); // mmm
  else
    return;

  // Selection AFTER the internal mouseUp sequence
  juce::MessageManager::callAsync([editor, newRange] {
    if (editor != nullptr)
      editor->setHighlightedRegion(newRange);
  });
}

void PlaybackTextPresenter::mouseWheelMove(
    const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) {
  if (wheel.deltaY == 0.0f)
    return;

  auto *editor = dynamic_cast<juce::TextEditor *>(event.eventComponent);
  if (editor == nullptr)
    return;

  if (editor->hasKeyboardFocus(true) ||
      (editor == &owner.elapsedTimeEditor && isEditingElapsed) ||
      (editor == &owner.remainingTimeEditor && isEditingRemaining) ||
      (editor == &owner.cutLengthEditor && isEditingLoopLength))
    return;

  double currentVal = TimeUtils::parseTime(editor->getText());
  if (currentVal < 0.0)
    currentVal = 0.0;

  // Determine step size
  int charIndex = editor->getTextIndexAt(event.getPosition());

  // Check if it's remaining time (starts with '-')
  bool isNegative = (editor == &owner.remainingTimeEditor) ||
                    editor->getText().startsWith("-");
  int offset = isNegative ? 1 : 0;
  int effectiveIndex = charIndex - offset;

  double sampleRate = 0.0;
  if (auto *reader = owner.getAudioPlayer().getAudioFormatReader())
    sampleRate = reader->sampleRate;

  double step = TimeEntryHelpers::calculateStepSize(effectiveIndex, event.mods,
                                                    sampleRate);

  double direction = (wheel.deltaY > 0) ? 1.0 : -1.0;
  double newVal = juce::jmax(0.0, currentVal + (direction * step));

  // The logic below needs update for Cut Mode!
  // If Cut Mode is active, "newVal" is relative.
  // We should probably just call applyTimeEdit concept here, but we are doing it via text value manipulation.

  if (owner.isCutModeActive())
  {
      // If we are editing Elapsed Time, newVal is desired elapsed relative to cutIn.
      // So absolute = cutIn + newVal.
      if (editor == &owner.elapsedTimeEditor) {
          double newAbs = owner.getCutInPosition() + newVal;
          owner.getAudioPlayer().setPositionConstrained(newAbs, owner.getCutInPosition(), owner.getCutOutPosition());
      }
      // If we are editing Remaining, newVal is desired remaining.
      // So absolute = cutOut - newVal.
      else if (editor == &owner.remainingTimeEditor) {
          double newAbs = owner.getCutOutPosition() - newVal;
          owner.getAudioPlayer().setPositionConstrained(newAbs, owner.getCutInPosition(), owner.getCutOutPosition());
      }
      else if (editor == &owner.cutLengthEditor) {
          owner.setCutOutPosition(owner.getCutInPosition() + newVal);
          owner.ensureCutOrder();
          owner.updateCutLabels();
      }
  }
  else
  {
      if (editor == &owner.elapsedTimeEditor) {
        owner.getAudioPlayer().getTransportSource().setPosition(newVal);
      } else if (editor == &owner.remainingTimeEditor) {
        double total = owner.getAudioPlayer().getThumbnail().getTotalLength();
        owner.getAudioPlayer().getTransportSource().setPosition(total - newVal);
      } else if (editor == &owner.cutLengthEditor) {
        owner.setCutOutPosition(owner.getCutInPosition() + newVal);
        owner.ensureCutOrder();
        owner.updateCutLabels();
      }
  }

  updateEditors();
}
