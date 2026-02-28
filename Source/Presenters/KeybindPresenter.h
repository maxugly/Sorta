#ifndef AUDIOFILER_KEYBINDPRESENTER_H
#define AUDIOFILER_KEYBINDPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file KeybindPresenter.h
 * @ingroup UI
 * @brief Coordinator for keyboard shortcuts and shortcut-to-action mapping.
 */

class ControlPanel;

/**
 * @class KeybindPresenter
 * @brief Handles keyboard shortcuts and routes them to state mutations or peer presenters.
 * 
 * @details In the Model-View-Presenter (MVP) architecture, the KeybindPresenter 
 *          serves as a specialized "glue" layer that translates low-level 
 *          keyboard events (View) into high-level business logic commands 
 *          (Model/SessionState). By decoupling the key mapping logic from the 
 *          main UI components, it allows for centralized hotkey management 
 *          and ensures that the Views remain focused on visual representation. 
 *          It acts as an orchestrator, determining which peer presenters or 
 *          state mutations should be triggered by a specific key sequence.
 * 
 * @see ControlPanel, InteractionCoordinator, SessionState
 */
class KeybindPresenter final {
  public:
    /**
     * @brief Constructs a new KeybindPresenter.
     * @param ownerPanel The parent ControlPanel used for routing UI actions.
     */
    explicit KeybindPresenter(ControlPanel &ownerPanel);

    /** 
     * @brief Evaluates a key press and dispatches the corresponding action. 
     * @param key The key event to process.
     * @return True if the key was handled, false otherwise.
     */
    bool handleKeyPress(const juce::KeyPress &key);

  private:
    /** @brief Handles global application shortcuts (e.g., Quit, Save). */
    bool handleGlobalKeybinds(const juce::KeyPress &key);

    /** @brief Handles shortcuts related to audio playback (e.g., Space for Play/Pause). */
    bool handlePlaybackKeybinds(const juce::KeyPress &key);

    /** @brief Handles shortcuts that toggle UI elements or views. */
    bool handleUIToggleKeybinds(const juce::KeyPress &key);

    /** @brief Handles shortcuts for manipulating cut boundaries. */
    bool handleCutKeybinds(const juce::KeyPress &key);

    ControlPanel &owner;
};

#endif
