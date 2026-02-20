#include "Presenters/ControlButtonsPresenter.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/MouseHandler.h"
#include "UI/Views/CutLayerView.h"
#include "UI/Views/WaveformView.h"
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
    owner.addAndMakeVisible(owner.openButton);
    owner.openButton.setButtonText(Config::Labels::openButton);
    owner.openButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    owner.openButton.onClick = [this] { owner.invokeOwnerOpenDialog(); };
}

void ControlButtonsPresenter::initialiseModeButton() {
    owner.addAndMakeVisible(owner.modeButton);
    owner.modeButton.setButtonText(Config::Labels::viewModeClassic);
    owner.modeButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);
    owner.modeButton.setClickingTogglesState(true);
    owner.modeButton.onClick = [this] {
        owner.currentMode = owner.modeButton.getToggleState() ? AppEnums::ViewMode::Overlay
                                                              : AppEnums::ViewMode::Classic;
        owner.modeButton.setButtonText(owner.currentMode == AppEnums::ViewMode::Classic
                                           ? Config::Labels::viewModeClassic
                                           : Config::Labels::viewModeOverlay);
        owner.resized();
        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseChannelViewButton() {
    owner.addAndMakeVisible(owner.channelViewButton);
    owner.channelViewButton.setButtonText(Config::Labels::channelViewMono);
    owner.channelViewButton.getProperties().set("GroupPosition",
                                                (int)AppEnums::GroupPosition::Right);
    owner.channelViewButton.setClickingTogglesState(true);
    owner.channelViewButton.onClick = [this] {
        owner.currentChannelViewMode = owner.channelViewButton.getToggleState()
                                           ? AppEnums::ChannelViewMode::Stereo
                                           : AppEnums::ChannelViewMode::Mono;
        owner.channelViewButton.setButtonText(owner.currentChannelViewMode ==
                                                      AppEnums::ChannelViewMode::Mono
                                                  ? Config::Labels::channelViewMono
                                                  : Config::Labels::channelViewStereo);
        if (owner.waveformView != nullptr)
            owner.waveformView->setChannelMode(owner.currentChannelViewMode);
        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseExitButton() {
    owner.addAndMakeVisible(owner.exitButton);
    owner.exitButton.setButtonText(Config::Labels::exitButton);
    owner.exitButton.setColour(juce::TextButton::buttonColourId, Config::Colors::Button::exit);
    owner.exitButton.onClick = [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };
}

void ControlButtonsPresenter::initialiseStatsButton() {
    owner.addAndMakeVisible(owner.statsButton);
    owner.statsButton.setButtonText(Config::Labels::statsButton);
    owner.statsButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    owner.statsButton.setClickingTogglesState(true);
    owner.statsButton.onClick = [this] {
        owner.setShouldShowStats(owner.statsButton.getToggleState());
        owner.updateComponentStates();
    };
}

void ControlButtonsPresenter::initialiseEyeCandyButton() {
    owner.addAndMakeVisible(owner.eyeCandyButton);
    owner.eyeCandyButton.setButtonText("*");
    owner.eyeCandyButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    owner.eyeCandyButton.setClickingTogglesState(true);
    owner.eyeCandyButton.setToggleState(owner.getInteractionCoordinator().shouldShowEyeCandy(),
                                        juce::dontSendNotification);
    owner.eyeCandyButton.onClick = [this] {
        owner.getInteractionCoordinator().setShouldShowEyeCandy(
            owner.eyeCandyButton.getToggleState());
        owner.repaint();
    };
}

void ControlButtonsPresenter::refreshStates() {
    owner.eyeCandyButton.setToggleState(owner.getInteractionCoordinator().shouldShowEyeCandy(),
                                        juce::dontSendNotification);
}
