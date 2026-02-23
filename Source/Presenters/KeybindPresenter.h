#ifndef AUDIOFILER_KEYBINDPRESENTER_H
#define AUDIOFILER_KEYBINDPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class ControlPanel;

/**
 * @class KeybindPresenter
 * @brief Handles keyboard shortcuts and routes them to state mutations or peer presenters.
 */
class KeybindPresenter final {
  public:
    explicit KeybindPresenter(ControlPanel &ownerPanel);

    bool handleKeyPress(const juce::KeyPress &key);

  private:
    bool handleGlobalKeybinds(const juce::KeyPress &key);
    bool handlePlaybackKeybinds(const juce::KeyPress &key);
    bool handleUIToggleKeybinds(const juce::KeyPress &key);
    bool handleCutKeybinds(const juce::KeyPress &key);

    ControlPanel &owner;
};

#endif
