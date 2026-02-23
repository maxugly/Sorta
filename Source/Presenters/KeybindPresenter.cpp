#include "Presenters/KeybindPresenter.h"

#include "Core/AppEnums.h"
#include "Core/AudioPlayer.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/TopBarView.h"
#include "Utils/Config.h"

KeybindPresenter::KeybindPresenter(ControlPanel &ownerPanel)
    : owner(ownerPanel) {
}

bool KeybindPresenter::handleKeyPress(const juce::KeyPress &key) {
    if (handleGlobalKeybinds(key))
        return true;

    if (owner.getAudioPlayer().getThumbnail().getTotalLength() > 0.0) {
        if (handlePlaybackKeybinds(key))
            return true;
        if (handleUIToggleKeybinds(key))
            return true;
        if (handleCutKeybinds(key))
            return true;
    }
    return false;
}

bool KeybindPresenter::handleGlobalKeybinds(const juce::KeyPress &key) {
    const juce::juce_wchar keyChar = key.getTextCharacter();
    if (keyChar == 'e' || keyChar == 'E') {
        if (auto *app = juce::JUCEApplication::getInstance())
            app->systemRequestedQuit();
        return true;
    }
    if (keyChar == 'd' || keyChar == 'D') {
        owner.invokeOwnerOpenDialog();
        return true;
    }
    return false;
}

bool KeybindPresenter::handlePlaybackKeybinds(const juce::KeyPress &key) {
    auto& audioPlayer = owner.getAudioPlayer();
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

bool KeybindPresenter::handleUIToggleKeybinds(const juce::KeyPress &key) {
    const auto keyChar = key.getTextCharacter();
    if (keyChar == 's' || keyChar == 'S') {
        owner.getPresenterCore().getStatsPresenter().toggleVisibility();
        owner.getPresenterCore().getControlStatePresenter().refreshStates();
        return true;
    }
    if (keyChar == 'v' || keyChar == 'V') {
        if (auto* tb = owner.getTopBarView())
            tb->modeButton.triggerClick();
        return true;
    }
    if (keyChar == 'c' || keyChar == 'C') {
        if (auto* tb = owner.getTopBarView())
            tb->channelViewButton.triggerClick();
        return true;
    }
    if (keyChar == 'r' || keyChar == 'R') {
        auto& audioPlayer = owner.getAudioPlayer();
        audioPlayer.setRepeating(!audioPlayer.isRepeating());
        owner.getPresenterCore().getControlStatePresenter().refreshStates();
        return true;
    }
    return false;
}

bool KeybindPresenter::handleCutKeybinds(const juce::KeyPress &key) {
    const auto keyChar = key.getTextCharacter();
    auto& sessionState = owner.getSessionState();
    auto& audioPlayer = owner.getAudioPlayer();
    auto& interactionCoordinator = owner.getInteractionCoordinator();

    if (interactionCoordinator.getPlacementMode() == AppEnums::PlacementMode::None) {
        if (keyChar == 'i' || keyChar == 'I') {
            sessionState.setCutIn(audioPlayer.getCurrentPosition());
            sessionState.setAutoCutInActive(false);
            interactionCoordinator.setNeedsJumpToCutIn(false);
            audioPlayer.setPlayheadPosition(sessionState.getCutIn());
            return true;
        }
        if (keyChar == 'o' || keyChar == 'O') {
            sessionState.setCutOut(audioPlayer.getCurrentPosition());
            sessionState.setAutoCutOutActive(false);
            interactionCoordinator.setNeedsJumpToCutIn(false);
            audioPlayer.setPlayheadPosition(sessionState.getCutIn());
            return true;
        }
    }
    if (keyChar == 'u' || keyChar == 'U') {
        owner.getPresenterCore().getCutResetPresenter().resetIn();
        return true;
    }
    if (keyChar == 'p' || keyChar == 'P') {
        owner.getPresenterCore().getCutResetPresenter().resetOut();
        return true;
    }
    return false;
}
