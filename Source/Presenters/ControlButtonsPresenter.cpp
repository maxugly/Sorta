#include "Presenters/ControlButtonsPresenter.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/CutLayerView.h"
#include "Utils/Config.h"
#include "Workers/SilenceDetector.h"

ControlButtonsPresenter::ControlButtonsPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
}

void ControlButtonsPresenter::initialiseAllButtons() {
    initialiseOpenButton();
    initialiseModeButton();
    initialiseChannelViewButton();
    initialiseExitButton();
    initialiseStatsButton();
    initialiseEyeCandyButton();
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

        if (owner.waveformCanvasView != nullptr)
            owner.waveformCanvasView->getWaveformView().setChannelMode(newMode);

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

void ControlButtonsPresenter::initialiseEyeCandyButton() {
    if (owner.topBarView == nullptr) return;
    auto& btn = owner.topBarView->eyeCandyButton;
    btn.setButtonText("*");
    btn.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    btn.setClickingTogglesState(true);
    btn.setToggleState(owner.getInteractionCoordinator().shouldShowEyeCandy(),
                                        juce::dontSendNotification);
    btn.onClick = [this] {
        if (owner.topBarView == nullptr) return;
        owner.getInteractionCoordinator().setShouldShowEyeCandy(
            owner.topBarView->eyeCandyButton.getToggleState());
        owner.repaint();
    };
}

void ControlButtonsPresenter::refreshStates() {
    if (owner.topBarView != nullptr)
        owner.topBarView->eyeCandyButton.setToggleState(owner.getInteractionCoordinator().shouldShowEyeCandy(),
                                            juce::dontSendNotification);
}
