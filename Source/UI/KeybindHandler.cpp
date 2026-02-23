

#include "UI/KeybindHandler.h"

#include "Core/AppEnums.h"
#include "Core/AudioPlayer.h"
#include "MainComponent.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

KeybindHandler::KeybindHandler(MainComponent &mainComponentIn, AudioPlayer &audioPlayerIn,
                               ControlPanel &controlPanelIn)
    : mainComponent(mainComponentIn), audioPlayer(audioPlayerIn), controlPanel(controlPanelIn) {
}

bool KeybindHandler::handleKeyPress(const juce::KeyPress &key) {
    if (handleGlobalKeybinds(key))
        return true;

    if (audioPlayer.getThumbnail().getTotalLength() > 0.0) {
        if (handlePlaybackKeybinds(key))
            return true;
        if (handleUIToggleKeybinds(key))
            return true;
        if (handleCutKeybinds(key))
            return true;
    }
    return false;
}

bool KeybindHandler::handleGlobalKeybinds(const juce::KeyPress &key) {
    const juce::juce_wchar keyChar = key.getTextCharacter();
    if (keyChar == 'e' || keyChar == 'E') {
        if (auto *app = juce::JUCEApplication::getInstance())
            app->systemRequestedQuit();
        return true;
    }
    if (keyChar == 'd' || keyChar == 'D') {
        mainComponent.openButtonClicked();
        return true;
    }
    return false;
}

bool KeybindHandler::handlePlaybackKeybinds(const juce::KeyPress &key) {
    if (key == juce::KeyPress::spaceKey) {
        audioPlayer.togglePlayStop();
        return true;
    }
    constexpr double seekStepSeconds = Config::Audio::keyboardSkipSeconds;
    if (key.getKeyCode() == juce::KeyPress::leftKey) {
        const double current = audioPlayer.getCurrentPosition();
        audioPlayer.setPlayheadPosition(current - seekStepSeconds);
        return true;
    }
    if (key.getKeyCode() == juce::KeyPress::rightKey) {
        const double current = audioPlayer.getCurrentPosition();
        audioPlayer.setPlayheadPosition(current + seekStepSeconds);
        return true;
    }
    return false;
}

bool KeybindHandler::handleUIToggleKeybinds(const juce::KeyPress &key) {
    const auto keyChar = key.getTextCharacter();
    if (keyChar == 's' || keyChar == 'S') {
        controlPanel.getPresenterCore().getStatsPresenter().toggleVisibility();
        controlPanel.getPresenterCore().getControlStatePresenter().refreshStates();
        return true;
    }
    if (keyChar == 'v' || keyChar == 'V') {
        if (auto* tb = controlPanel.getTopBarView())
            tb->modeButton.triggerClick();
        return true;
    }
    if (keyChar == 'c' || keyChar == 'C') {
        if (auto* tb = controlPanel.getTopBarView())
            tb->channelViewButton.triggerClick();
        return true;
    }
    if (keyChar == 'r' || keyChar == 'R') {
        audioPlayer.setRepeating(!audioPlayer.isRepeating());
        controlPanel.getPresenterCore().getControlStatePresenter().refreshStates();
        return true;
    }
    return false;
}

bool KeybindHandler::handleCutKeybinds(const juce::KeyPress &key) {
    const auto keyChar = key.getTextCharacter();
    if (controlPanel.getInteractionCoordinator().getPlacementMode() == AppEnums::PlacementMode::None) {
        if (keyChar == 'i' || keyChar == 'I') {
            controlPanel.getSessionState().setCutIn(audioPlayer.getCurrentPosition());
            controlPanel.getSessionState().setAutoCutInActive(false);
            controlPanel.getInteractionCoordinator().setNeedsJumpToCutIn(false);
            controlPanel.getAudioPlayer().setPlayheadPosition(controlPanel.getSessionState().getCutIn());
            controlPanel.repaint();
            return true;
        }
        if (keyChar == 'o' || keyChar == 'O') {
            controlPanel.getSessionState().setCutOut(audioPlayer.getCurrentPosition());
            controlPanel.getSessionState().setAutoCutOutActive(false);
            controlPanel.getInteractionCoordinator().setNeedsJumpToCutIn(false);
            controlPanel.getAudioPlayer().setPlayheadPosition(controlPanel.getSessionState().getCutIn());
            controlPanel.repaint();
            return true;
        }
    }
    if (keyChar == 'u' || keyChar == 'U') {
        controlPanel.getPresenterCore().getCutResetPresenter().resetIn();
        return true;
    }
    if (keyChar == 'p' || keyChar == 'P') {
        controlPanel.getPresenterCore().getCutResetPresenter().resetOut();
        return true;
    }
    return false;
}
