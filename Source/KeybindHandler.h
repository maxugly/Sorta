#ifndef AUDIOFILER_KEYBINDHANDLER_H
#define AUDIOFILER_KEYBINDHANDLER_H

#include <JuceHeader.h>

class MainComponent;
class AudioPlayer;
class ControlPanel;

/**
 * @class KeybindHandler
 * @brief Manages keyboard shortcuts for the application.
 */
class KeybindHandler final
{
public:
    KeybindHandler(MainComponent& mainComponentIn, AudioPlayer& audioPlayerIn, ControlPanel& controlPanelIn);

    /**
     * @brief Central entry point for key press handling.
     */
    bool handleKeyPress(const juce::KeyPress& key);

private:
    bool handleGlobalKeybinds(const juce::KeyPress& key);
    bool handlePlaybackKeybinds(const juce::KeyPress& key);
    bool handleUIToggleKeybinds(const juce::KeyPress& key);
    bool handleCutKeybinds(const juce::KeyPress& key);

    MainComponent& mainComponent;
    AudioPlayer& audioPlayer;
    ControlPanel& controlPanel;
};

#endif // AUDIOFILER_KEYBINDHANDLER_H
