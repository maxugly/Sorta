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
    Config::initializeConfigs();

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
    
    dependencyContainer = std::make_unique<DependencyContainer>(*controlPanel);
    controlPanel->setDependencies(*dependencyContainer);
    
    // 5. Finalise setup now that all dependencies are present
    controlPanel->injectLogic(*interactionCoordinator, *playbackTimerManager, *dependencyContainer, *focusManager);
    
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
        Config::Labels::selectAudio, 
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*" // Allow selecting anything; we will filter manually for robustness
    );

    auto flags = juce::FileBrowserComponent::openMode | 
                 juce::FileBrowserComponent::canSelectDirectories | 
                 juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(flags, [this](const juce::FileChooser &fc) {
        auto result = fc.getResult();
        if (result.exists()) {
            juce::Array<juce::File> audioFiles;
            auto& formatManager = audioPlayer->getFormatManager();

            if (result.isDirectory()) {
                // Safe, format-aware directory scan
                auto allFiles = result.findChildFiles(juce::File::findFiles, false, "*");
                for (const auto& f : allFiles) {
                    if (formatManager.findFormatForFileExtension(f.getFileExtension()) != nullptr) {
                        audioFiles.add(f);
                    }
                }
                audioFiles.sort(); // Ensure consistent alphabetical ordering
            } else {
                if (formatManager.findFormatForFileExtension(result.getFileExtension()) != nullptr) {
                    audioFiles.add(result);
                }
            }

            if (!audioFiles.isEmpty()) {
                // 1. Load the first file immediately for playback
                auto loadResult = audioPlayer->loadFile(audioFiles.getFirst());
                
                if (loadResult.wasOk()) {
                    // 2. Queue the remaining files for the background worker
                    std::vector<juce::String> queue;
                    for (int i = 1; i < audioFiles.size(); ++i) {
                        queue.push_back(audioFiles[i].getFullPathName());
                    }
                    sessionState.setFileQueue(queue);
                } else {
                    dependencyContainer->getStatsPresenter().setDisplayText(
                        loadResult.getErrorMessage(), Config::Colors::statsErrorText);
                }
            } else {
                // Alert the user if they opened an empty or unsupported folder
                dependencyContainer->getStatsPresenter().setDisplayText(
                    Config::Labels::errorNoValidFiles, Config::Colors::statsErrorText);
            }
        }

        grabKeyboardFocus();
    });
}

bool MainComponent::keyPressed(const juce::KeyPress &key) {
    if (controlPanel != nullptr)
        return controlPanel->getDependencies().getKeybindPresenter().handleKeyPress(key);
    return false;
}
