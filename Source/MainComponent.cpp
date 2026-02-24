#include "MainComponent.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/KeybindPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/PlaybackTimeView.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

MainComponent::MainComponent() {
    audioPlayer = std::make_unique<AudioPlayer>(sessionState);

    // 1. Instantiate managers that don't depend on ControlPanel
    interactionCoordinator = std::make_unique<InteractionCoordinator>();
    playbackTimerManager = std::make_unique<PlaybackTimerManager>(sessionState, *audioPlayer, *interactionCoordinator);
    
    // 2. Instantiate the visual container
    controlPanel = std::make_unique<ControlPanel>(*this, sessionState);
    
    // 3. Partial injection to allow presenters to access core logic during construction
    controlPanel->setInteractionCoordinator(*interactionCoordinator);
    controlPanel->setPlaybackTimerManager(*playbackTimerManager);

    // 4. Instantiate managers that DO depend on ControlPanel
    focusManager = std::make_unique<FocusManager>(*controlPanel);
    controlPanel->setFocusManager(*focusManager);
    
    presenterCore = std::make_unique<PresenterCore>(*controlPanel);
    controlPanel->setPresenterCore(*presenterCore);
    
    // 5. Finalise setup now that all dependencies are present
    controlPanel->injectLogic(*interactionCoordinator, *playbackTimerManager, *presenterCore, *focusManager);
    
    addAndMakeVisible(controlPanel.get());

    setAudioChannels(0, 2);

    setSize(Config::Layout::Window::width, Config::Layout::Window::height);

    setWantsKeyboardFocus(true);
    openGLContext.attachTo(*this);
}

MainComponent::~MainComponent() {
    openGLContext.detach();

    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    audioPlayer->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    audioPlayer->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    audioPlayer->releaseResources();
}

void MainComponent::paint(juce::Graphics &g) {
    g.fillAll(Config::Colors::Window::background);
}

void MainComponent::resized() {
    if (controlPanel != nullptr)
        controlPanel->setBounds(getLocalBounds());
}

void MainComponent::openButtonClicked() {
    chooser = std::make_unique<juce::FileChooser>(
        Config::Labels::selectAudio, juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        audioPlayer->getFormatManager().getWildcardForAllFormats());

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(flags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file.exists()) {
            auto result = audioPlayer->loadFile(file);
            if (result.wasOk()) {
                // Success - PlaybackTextPresenter handles UI updates
            } else {
                presenterCore->getStatsPresenter().setDisplayText(
                    result.getErrorMessage(), Config::Colors::statsErrorText);
            }
        }

        grabKeyboardFocus();
    });
}

bool MainComponent::keyPressed(const juce::KeyPress &key) {
    if (controlPanel != nullptr)
        return controlPanel->getPresenterCore().getKeybindPresenter().handleKeyPress(key);
    return false;
}
