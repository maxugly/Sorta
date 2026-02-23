#ifndef AUDIOFILER_MAINCOMPONENT_H
#define AUDIOFILER_MAINCOMPONENT_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "UI/ControlPanel.h"
#include "UI/InteractionCoordinator.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Presenters/PresenterCore.h"
#include "UI/FocusManager.h"

/**
 * @file MainComponent.h
 * @ingroup UI
 * @brief The root shell component that owns all top-level managers and the visual container.
 */
class MainComponent : public juce::AudioAppComponent {
  public:
    using PlacementMode = AppEnums::PlacementMode;

    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress &key) override;

    void openButtonClicked();

    AudioPlayer *getAudioPlayer() const { return audioPlayer.get(); }
    InteractionCoordinator& getInteractionCoordinator() { return *interactionCoordinator; }
    PlaybackTimerManager& getPlaybackTimerManager() { return *playbackTimerManager; }

  private:
    SessionState sessionState;
    std::unique_ptr<AudioPlayer> audioPlayer;
    
    // Core Logic Managers (Owned by the Shell)
    std::unique_ptr<InteractionCoordinator> interactionCoordinator;
    std::unique_ptr<PlaybackTimerManager> playbackTimerManager;
    std::unique_ptr<PresenterCore> presenterCore;
    std::unique_ptr<FocusManager> focusManager;

    std::unique_ptr<juce::FileChooser> chooser;
    std::unique_ptr<ControlPanel> controlPanel;
    juce::OpenGLContext openGLContext;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

#endif
