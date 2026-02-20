#include "ControlButtonsPresenter.h"

#include "ControlPanel.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/CutLayerView.h"
#include "ControlPanelCopy.h"
#include "TransportPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "Presenters/RepeatPresenter.h"
#include "CutResetPresenter.h"
#include "MouseHandler.h"
#include "SilenceDetector.h"

ControlButtonsPresenter::ControlButtonsPresenter(ControlPanel& ownerPanel)
    : owner(ownerPanel)
{
}

void ControlButtonsPresenter::initialiseAllButtons()
{
    initialiseOpenButton();
    initialiseModeButton();
    initialiseChannelViewButton();
    initialiseExitButton();
    initialiseStatsButton();
    initialiseEyeCandyButton();
}

void ControlButtonsPresenter::initialiseOpenButton()
{
    owner.addAndMakeVisible(owner.openButton);
    owner.openButton.setButtonText(ControlPanelCopy::openButtonText());
    owner.openButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    owner.openButton.onClick = [this] { owner.invokeOwnerOpenDialog(); };
}

void ControlButtonsPresenter::initialiseModeButton()
{
    owner.addAndMakeVisible(owner.modeButton);
    owner.modeButton.setButtonText(ControlPanelCopy::viewModeClassicText());
    owner.modeButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);
    owner.modeButton.setClickingTogglesState(true);
    owner.modeButton.onClick = [this] {
        owner.currentMode = owner.modeButton.getToggleState() ? AppEnums::ViewMode::Overlay : AppEnums::ViewMode::Classic;
        owner.modeButton.setButtonText(owner.currentMode == AppEnums::ViewMode::Classic
                                       ? ControlPanelCopy::viewModeClassicText()
                                       : ControlPanelCopy::viewModeOverlayText());
        owner.resized();
        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseChannelViewButton()
{
    owner.addAndMakeVisible(owner.channelViewButton);
    owner.channelViewButton.setButtonText(ControlPanelCopy::channelViewMonoText());
    owner.channelViewButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
    owner.channelViewButton.setClickingTogglesState(true);
    owner.channelViewButton.onClick = [this] {
        owner.currentChannelViewMode = owner.channelViewButton.getToggleState()
            ? AppEnums::ChannelViewMode::Stereo
            : AppEnums::ChannelViewMode::Mono;
        owner.channelViewButton.setButtonText(owner.currentChannelViewMode == AppEnums::ChannelViewMode::Mono
                                              ? ControlPanelCopy::channelViewMonoText()
                                              : ControlPanelCopy::channelViewStereoText());
        if (owner.waveformView != nullptr)
            owner.waveformView->setChannelMode(owner.currentChannelViewMode);
        owner.repaint();
    };
}

void ControlButtonsPresenter::initialiseExitButton()
{
    owner.addAndMakeVisible(owner.exitButton);
    owner.exitButton.setButtonText(ControlPanelCopy::exitButtonText());
    owner.exitButton.setColour(juce::TextButton::buttonColourId, Config::Colors::Button::exit);
    owner.exitButton.onClick = [] {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    };
}

void ControlButtonsPresenter::initialiseStatsButton()
{
    owner.addAndMakeVisible(owner.statsButton);
    owner.statsButton.setButtonText(ControlPanelCopy::statsButtonText());
    owner.statsButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    owner.statsButton.setClickingTogglesState(true);
    owner.statsButton.onClick = [this] {
        owner.setShouldShowStats(owner.statsButton.getToggleState());
        owner.updateComponentStates();
    };
}

void ControlButtonsPresenter::initialiseEyeCandyButton()
{
    owner.addAndMakeVisible(owner.eyeCandyButton);
    owner.eyeCandyButton.setButtonText("*");
    owner.eyeCandyButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Alone);
    owner.eyeCandyButton.setClickingTogglesState(true);
    owner.eyeCandyButton.setToggleState(owner.getShowEyeCandy(), juce::dontSendNotification);
    owner.eyeCandyButton.onClick = [this] {
        owner.repaint();
    };
}
