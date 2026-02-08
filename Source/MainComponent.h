#pragma once

#include <JuceHeader.h>
#include "AudioPlayer.h"
#include "ControlPanel.h" 
#include "AppEnums.h"

class MainComponent  : public juce::AudioAppComponent,
                       public juce::ChangeListener,
                       public juce::Timer
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
  void timerCallback() override;

  /**
   * @brief Handles key press events.
   * @param key The key that was pressed.
   * @return True if the key event was handled, false otherwise.
   */
  bool keyPressed (const juce::KeyPress& key) override;
  
  /**
   * @brief Initiates the file open dialog.
   */
  void openButtonClicked();

  /**
   * @brief Seeks the audio playback position based on an X-coordinate in the waveform.
   *        This method is still part of MainComponent as it directly interacts with AudioPlayer
   *        but is now called by ControlPanel's mouse event handlers.
   * @param x The X-coordinate in pixels relative to the MainComponent.
   */
  void seekToPosition (int x);

  /**
   * @brief Formats a time in seconds into a human-readable string (HH:MM:SS:mmm).
   * @param seconds The time in seconds.
   * @return A formatted juce::String.
   */
  juce::String formatTime(double seconds);
  
  AudioPlayer* getAudioPlayer() const { return audioPlayer.get(); }

private:
    std::unique_ptr<AudioPlayer> audioPlayer;
    std::unique_ptr<juce::FileChooser> chooser;
    std::unique_ptr<ControlPanel> controlPanel;
    
    // Keypress handlers
    bool handleGlobalKeybinds(const juce::KeyPress& key);
    bool handlePlaybackKeybinds(const juce::KeyPress& key);
    bool handleUIToggleKeybinds(const juce::KeyPress& key);
    bool handleLoopKeybinds(const juce::KeyPress& key);

    /**
     * @brief Builds a string containing various audio statistics.
     * @return A juce::String with formatted audio statistics.
     */
    juce::String buildStatsString();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};