

#include "Presenters/ControlStatePresenter.h"

#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "Workers/SilenceDetector.h"

ControlStatePresenter::ControlStatePresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
}

void ControlStatePresenter::refreshStates() {
    const bool enabled = owner.getAudioPlayer().getThumbnail().getTotalLength() > 0.0;

    updateGeneralButtonStates(enabled);
    updateCutModeControlStates(owner.isCutModeActive(), enabled);

    owner.getBoundaryLogicPresenter().refreshLabels();
    owner.getPlaybackTextPresenter().updateEditors();
    owner.getRepeatButtonPresenter().cutPreferenceChanged(owner.getSessionState().getCutPrefs());
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
        owner.inStrip->setEnabled(enabled && isCutModeActive);
        owner.inStrip->setVisible(isCutModeActive);
    }

    if (owner.outStrip != nullptr) {
        owner.outStrip->setEnabled(enabled && isCutModeActive);
        owner.outStrip->setVisible(isCutModeActive);
    }

    if (owner.playbackTimeView != nullptr) {
        auto &length = owner.playbackTimeView->getCutLengthEditor();
        length.setEnabled(enabled && isCutModeActive);
        length.setVisible(isCutModeActive);
    }

    if (owner.inStrip != nullptr)
        owner.inStrip->updateAutoCutState(owner.getSilenceDetector().getIsAutoCutInActive());
    if (owner.outStrip != nullptr)
        owner.outStrip->updateAutoCutState(owner.getSilenceDetector().getIsAutoCutOutActive());
}
