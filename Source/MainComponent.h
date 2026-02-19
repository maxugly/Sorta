#ifndef AUDIOFILER_MAINCOMPONENT_H
#define AUDIOFILER_MAINCOMPONENT_H

/**
 * @defgroup UI UI Components
 * @brief Classes responsible for the user interface, rendering, and user interaction.
 *
 * @defgroup Audio Audio Engine
 * @brief Core audio playback, file handling, and processing logic.
 *
 * @defgroup State State Management
 * @brief Application state, configuration, and data models.
 *
 * @defgroup Workers Threading and Workers
 * @brief Background threads and worker classes for long-running tasks.
 */

#if defined(JUCE_HEADLESS)
    #include <juce_gui_extra/juce_gui_extra.h>
    #include <juce_opengl/juce_opengl.h>
#else
    #include <JuceHeader.h>
#endif

#include "AudioPlayer.h"
#include "ControlPanel.h" 
#include "AppEnums.h"
#include "SessionState.h"

class KeybindHandler;

class PlaybackRepeatController;

/**
 * @ingroup UI
 * @class MainComponent
 * @brief The main application window and entry point for the UI.
 * @details Manages the high-level component hierarchy, including the AudioPlayer,
 * ControlPanel, and SessionState. It also handles global keybinds and
 * OpenGL context management.
 * @see AudioPlayer
 * @see ControlPanel
 * @see SessionState
 */
class MainComponent  : public juce::AudioAppComponent,
                    public juce::ChangeListener
{
public:
  using PlacementMode = AppEnums::PlacementMode; 

  MainComponent();

  ~MainComponent() override;

  void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

  void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

  void releaseResources() override;

  void paint(juce::Graphics& g) override;

  void resized() override;

  void changeListenerCallback(juce::ChangeBroadcaster* source) override;

  bool keyPressed (const juce::KeyPress& key) override;

  void openButtonClicked();

  void seekToPosition (int x);

  AudioPlayer* getAudioPlayer() const { return audioPlayer.get(); }

private:

    /** @brief The central data model for the application. */
    SessionState sessionState;
    /** @brief The audio engine responsible for playback and file management. */
    std::unique_ptr<AudioPlayer> audioPlayer;       
    std::unique_ptr<juce::FileChooser> chooser;     
    /** @brief The main UI container for playback controls and waveform visualization. */
    std::unique_ptr<ControlPanel> controlPanel;     
    std::unique_ptr<KeybindHandler> keybindHandler; 
    std::unique_ptr<PlaybackRepeatController> playbackRepeatController; 
    juce::OpenGLContext openGLContext; 

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

#endif 
