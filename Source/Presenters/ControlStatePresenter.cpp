

#include "Presenters/ControlStatePresenter.h"

#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/ZoomView.h"

ControlStatePresenter::ControlStatePresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
    owner.getSessionState().addListener(this);
    owner.getAudioPlayer().addChangeListener(this);
}

ControlStatePresenter::~ControlStatePresenter() {
    owner.getAudioPlayer().removeChangeListener(this);
    owner.getSessionState().removeListener(this);
}

void ControlStatePresenter::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &owner.getAudioPlayer()) {
        if (auto* ts = owner.getTransportStrip())
            ts->updatePlayButtonText(owner.getAudioPlayer().isPlaying());
    }
}

void ControlStatePresenter::refreshStates() {
    const bool enabled = owner.getAudioPlayer().getThumbnail().getTotalLength() > 0.0;

    updateGeneralButtonStates(enabled);
    updateCutModeControlStates(owner.getSessionState().getCutPrefs().active, enabled);

    owner.getBoundaryLogicPresenter().refreshLabels();
    owner.getPlaybackTextPresenter().updateEditors();
    owner.getRepeatButtonPresenter().cutPreferenceChanged(owner.getSessionState().getCutPrefs());
}

void ControlStatePresenter::fileChanged(const juce::String &filePath) {
    juce::ignoreUnused(filePath);
    updateUIFromState();
}

void ControlStatePresenter::updateUIFromState() {
    const auto &prefs = owner.getSessionState().getCutPrefs();
    const auto &autoCut = prefs.autoCut;

    if (auto* ts = owner.getTransportStrip()) {
        ts->updateAutoplayState(prefs.autoplay);
        ts->updateCutModeState(prefs.active);
    }

    if (owner.getInStrip() != nullptr)
        owner.getInStrip()->updateAutoCutState(autoCut.inActive);
    if (owner.getOutStrip() != nullptr)
        owner.getOutStrip()->updateAutoCutState(autoCut.outActive);

    const int inPercent = static_cast<int>(autoCut.thresholdIn * 100.0f);
    const int outPercent = static_cast<int>(autoCut.thresholdOut * 100.0f);

    if (owner.getInStrip() != nullptr)
        owner.getInStrip()->getThresholdEditor().setText(juce::String(inPercent), juce::dontSendNotification);
    if (owner.getOutStrip() != nullptr)
        owner.getOutStrip()->getThresholdEditor().setText(juce::String(outPercent), juce::dontSendNotification);

    refreshStates();

    owner.getBoundaryLogicPresenter().refreshLabels();
    owner.getPlaybackTextPresenter().updateEditors();

    if (owner.waveformCanvasView != nullptr)
        owner.waveformCanvasView->getZoomView().repaint();

    owner.repaint();
}

void ControlStatePresenter::cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
    if (auto* ts = owner.getTransportStrip()) {
        ts->updateAutoplayState(prefs.autoplay);
        ts->updateCutModeState(prefs.active);
    }

    if (owner.getInStrip() != nullptr)
        owner.getInStrip()->updateAutoCutState(prefs.autoCut.inActive);
    if (owner.getOutStrip() != nullptr)
        owner.getOutStrip()->updateAutoCutState(prefs.autoCut.outActive);

    refreshStates();
}

void ControlStatePresenter::cutInChanged(double value) {
    juce::ignoreUnused(value);
}

void ControlStatePresenter::cutOutChanged(double value) {
    juce::ignoreUnused(value);
}

void ControlStatePresenter::updateGeneralButtonStates(bool enabled) {
    if (owner.topBarView != nullptr) {
        owner.topBarView->openButton.setEnabled(true);
        owner.topBarView->exitButton.setEnabled(true);
    }

    if (auto *ts = owner.getTransportStrip()) {
        ts->getRepeatButton().setEnabled(true);
        ts->getAutoplayButton().setEnabled(true);
        ts->getCutButton().setEnabled(true);
        ts->getPlayStopButton().setEnabled(enabled);
        ts->getStopButton().setEnabled(enabled);
    }

    if (owner.topBarView != nullptr) {
        owner.topBarView->modeButton.setEnabled(enabled);
        owner.topBarView->statsButton.setEnabled(enabled);
        owner.topBarView->statsButton.setToggleState(
            owner.getPresenterCore().getStatsPresenter().isShowingStats(),
            juce::dontSendNotification);
        owner.topBarView->channelViewButton.setEnabled(enabled);
    }

    if (owner.playbackTimeView != nullptr) {
        auto &elapsed = owner.playbackTimeView->getElapsedEditor();
        auto &remaining = owner.playbackTimeView->getRemainingEditor();

        elapsed.setEnabled(enabled);
        remaining.setEnabled(enabled);
        elapsed.setVisible(enabled);
        remaining.setVisible(enabled);
    }

    owner.getPresenterCore().getStatsPresenter().setDisplayEnabled(enabled);
}

void ControlStatePresenter::updateCutModeControlStates(bool isCutModeActive, bool enabled) {
    if (owner.inStrip != nullptr) {
        owner.inStrip->setEnabled(isCutModeActive);
        owner.inStrip->setVisible(isCutModeActive);

        owner.inStrip->getMarkerButton().setEnabled(enabled);
        owner.inStrip->getTimerEditor().setEnabled(enabled);
        owner.inStrip->getResetButton().setEnabled(enabled);
    }

    if (owner.outStrip != nullptr) {
        owner.outStrip->setEnabled(isCutModeActive);
        owner.outStrip->setVisible(isCutModeActive);

        owner.outStrip->getMarkerButton().setEnabled(enabled);
        owner.outStrip->getTimerEditor().setEnabled(enabled);
        owner.outStrip->getResetButton().setEnabled(enabled);
    }

    if (owner.getCutLengthStrip() != nullptr) {
        owner.getCutLengthStrip()->setEnabled(isCutModeActive);
        owner.getCutLengthStrip()->setVisible(isCutModeActive);
        owner.getCutLengthStrip()->getLengthEditor().setEnabled(enabled);
        owner.getCutLengthStrip()->getLockButton().setEnabled(enabled);
    }

    if (owner.inStrip != nullptr)
        owner.inStrip->updateAutoCutState(owner.getSessionState().getCutPrefs().autoCut.inActive);
    if (owner.outStrip != nullptr)
        owner.outStrip->updateAutoCutState(owner.getSessionState().getCutPrefs().autoCut.outActive);
}
