#include "Presenters/ControlButtonsPresenter.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Components/MarkerStrip.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/CutLayerView.h"
#include "Utils/Config.h"

ControlButtonsPresenter::ControlButtonsPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
}

void ControlButtonsPresenter::initialiseAllButtons() {
    initialiseOpenButton();
    initialiseModeButton();
    initialiseChannelViewButton();
    initialiseExitButton();
    initialiseStatsButton();
    initialisePlayStopButton();
    initialiseStopButton();
    initialiseAutoplayButton();
    initialiseCutButton();
    initialiseMarkerButtons();
}

void ControlButtonsPresenter::initialiseOpenButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->openButton;
    btn.setButtonText(Config::Labels::openButton);
    btn.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    btn.onClick = [this] { owner.invokeOwnerOpenDialog(); };
}

void ControlButtonsPresenter::initialiseModeButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->modeButton;
    btn.setButtonText(Config::Labels::viewModeClassic);
    btn.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);
    btn.setClickingTogglesState(true);
    btn.onClick = [this] { 
        auto& session = owner.getSessionState();
        auto newMode = (session.getViewMode() == AppEnums::ViewMode::Classic) 
                       ? AppEnums::ViewMode::Overlay 
                       : AppEnums::ViewMode::Classic;
        session.setViewMode(newMode);

        if (owner.topBarView != nullptr) {
            owner.topBarView->modeButton.setToggleState(newMode == AppEnums::ViewMode::Overlay,
                                                  juce::dontSendNotification);
            owner.topBarView->modeButton.setButtonText(newMode == AppEnums::ViewMode::Classic
                                                     ? Config::Labels::viewModeClassic
                                                     : Config::Labels::viewModeOverlay);
        }
        owner.resized();
        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseChannelViewButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->channelViewButton;
    btn.setButtonText(Config::Labels::channelViewMono);
    btn.getProperties().set("GroupPosition",
                                                (int)AppEnums::GroupPosition::Right);
    btn.setClickingTogglesState(true);
    btn.onClick = [this] { 
        auto& session = owner.getSessionState();
        auto newMode = (session.getChannelViewMode() == AppEnums::ChannelViewMode::Mono)
                       ? AppEnums::ChannelViewMode::Stereo
                       : AppEnums::ChannelViewMode::Mono;
        session.setChannelViewMode(newMode);

        if (owner.topBarView != nullptr) {
            owner.topBarView->channelViewButton.setToggleState(
                newMode == AppEnums::ChannelViewMode::Stereo, juce::dontSendNotification);
            owner.topBarView->channelViewButton.setButtonText(
                newMode == AppEnums::ChannelViewMode::Mono
                    ? Config::Labels::channelViewMono
                    : Config::Labels::channelViewStereo);
        }

        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseExitButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->exitButton;
    btn.setButtonText(Config::Labels::exitButton);
    btn.setColour(juce::TextButton::buttonColourId, Config::Colors::Button::exit);
    btn.onClick = [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };
}

void ControlButtonsPresenter::initialiseStatsButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->statsButton;
    btn.setButtonText(Config::Labels::statsButton);
    btn.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    btn.setClickingTogglesState(true);
    btn.onClick = [this] {
        if (owner.topBarView == nullptr) return;
        owner.getPresenterCore().getStatsPresenter().setShouldShowStats(owner.topBarView->statsButton.getToggleState());
        owner.getPresenterCore().getControlStatePresenter().refreshStates();
    };
}

void ControlButtonsPresenter::initialisePlayStopButton() {
    if (auto* ts = owner.getTransportStrip()) {
        ts->getPlayStopButton().onClick = [this] {
            owner.getAudioPlayer().togglePlayStop();
        };
    }
}

void ControlButtonsPresenter::initialiseStopButton() {
    if (auto* ts = owner.getTransportStrip()) {
        ts->getStopButton().onClick = [this] {
            owner.getAudioPlayer().stopPlaybackAndReset();
            owner.getSessionState().setAutoPlayActive(false);
        };
    }
}

void ControlButtonsPresenter::initialiseAutoplayButton() {
    if (auto* ts = owner.getTransportStrip()) {
        auto& btn = ts->getAutoplayButton();
        btn.setToggleState(owner.getSessionState().getCutPrefs().autoplay, juce::dontSendNotification);
        btn.onClick = [this, &btn] {
            owner.getSessionState().setAutoPlayActive(btn.getToggleState());
        };
    }
}

void ControlButtonsPresenter::initialiseCutButton() {
    if (auto* ts = owner.getTransportStrip()) {
        auto& btn = ts->getCutButton();
        btn.setToggleState(owner.getSessionState().getCutPrefs().active, juce::dontSendNotification);
        btn.onClick = [this, &btn] {
            const bool active = btn.getToggleState();
            owner.getSessionState().setCutActive(active);

            if (active && owner.getAudioPlayer().isPlaying()) {
                const double pos = owner.getAudioPlayer().getCurrentPosition();
                const double cutIn = owner.getSessionState().getCutIn();
                const double cutOut = owner.getSessionState().getCutOut();
                if (pos < cutIn || pos >= cutOut)
                    owner.getAudioPlayer().setPlayheadPosition(cutIn);
            }
        };
    }
}

void ControlButtonsPresenter::initialiseMarkerButtons() {
    auto* inStrip = owner.getInStrip();
    auto* outStrip = owner.getOutStrip();
    auto& audioPlayer = owner.getAudioPlayer();
    auto& sessionState = owner.getSessionState();

    if (inStrip != nullptr) {
        inStrip->getMarkerButton().onLeftClick = [this, &audioPlayer] {
            audioPlayer.setCutIn(audioPlayer.getCurrentPosition());
        };
        inStrip->getMarkerButton().onRightClick = [this] {
            owner.getInteractionCoordinator().setPlacementMode(AppEnums::PlacementMode::CutIn);
        };
        inStrip->getResetButton().onClick = [this] {
            owner.getPresenterCore().getCutResetPresenter().resetIn();
        };
        inStrip->getAutoCutButton().onClick = [this, inStrip, &sessionState] {
            sessionState.setAutoCutInActive(inStrip->getAutoCutButton().getToggleState());
        };
    }

    if (outStrip != nullptr) {
        outStrip->getMarkerButton().onLeftClick = [this, &audioPlayer] {
            audioPlayer.setCutOut(audioPlayer.getCurrentPosition());
        };
        outStrip->getMarkerButton().onRightClick = [this] {
            owner.getInteractionCoordinator().setPlacementMode(AppEnums::PlacementMode::CutOut);
        };
        outStrip->getResetButton().onClick = [this] {
            owner.getPresenterCore().getCutResetPresenter().resetOut();
        };
        outStrip->getAutoCutButton().onClick = [this, outStrip, &sessionState] {
            sessionState.setAutoCutOutActive(outStrip->getAutoCutButton().getToggleState());
        };
    }
}

void ControlButtonsPresenter::refreshStates() {
}
