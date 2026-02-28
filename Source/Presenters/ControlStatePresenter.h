#ifndef AUDIOFILER_CONTROLSTATEPRESENTER_H
#define AUDIOFILER_CONTROLSTATEPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

/**
 * @file ControlStatePresenter.h
 * @ingroup UI
 * @brief Presenter for managing the enabled/disabled state of control panel buttons.
 */

class ControlPanel;

/**
 * @class ControlStatePresenter
 * @brief Manages the visual enabled/disabled states of the control panel UI.
 * 
 * @details Architecturally, the ControlStatePresenter follows the Model-View-Presenter (MVP) law, 
 *          serving as the "glue" that monitors the SessionState (Model) and 
 *          orchestrates the interactive state of the ControlPanel (View). It ensures 
 *          that buttons and controls are only active when they are contextually 
 *          relevant (e.g., disabling 'Cut' when no file is loaded). By abstracting 
 *          this conditional logic away from the View, it maintains a clean 
 *          separation of concerns and keeps the UI components "dumb."
 * 
 * @see SessionState, ControlPanel, InteractionCoordinator
 */
class ControlStatePresenter final : public SessionState::Listener, public juce::ChangeListener {
  public:
    /**
     * @brief Constructs a new ControlStatePresenter.
     * @param ownerPanel The parent ControlPanel whose state is being managed.
     */
    explicit ControlStatePresenter(ControlPanel &ownerPanel);

    /** @brief Destructor. */
    ~ControlStatePresenter() override;

    /** @brief Refreshes all button states from the current SessionState. */
    void refreshStates();

    /** @brief Synchronizes the UI component states with the internal presenter logic. */
    void updateUIFromState();

    /** 
     * @brief Standard JUCE change listener callback. 
     * @param source The broadcaster that triggered the change.
     */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    /** @brief Triggered when the active audio file changes. */
    void fileChanged(const juce::String &filePath) override;

    /** @brief Triggered when cut preferences change. */
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

    /** @brief Triggered when the 'In' point changes. */
    void cutInChanged(double value) override;

    /** @brief Triggered when the 'Out' point changes. */
    void cutOutChanged(double value) override;

  private:
    /** 
     * @brief Updates the enabled state of general buttons (Play, Stop, etc.). 
     * @param enabled Whether the buttons should be active.
     */
    void updateGeneralButtonStates(bool enabled);

    /** 
     * @brief Updates the enabled state of cut-mode specific controls. 
     * @param isCutModeActive Whether cut mode is currently engaged.
     * @param enabled Whether the controls should be interactive.
     */
    void updateCutModeControlStates(bool isCutModeActive, bool enabled);

    ControlPanel &owner;
};

#endif
