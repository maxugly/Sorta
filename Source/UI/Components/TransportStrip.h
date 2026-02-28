#ifndef AUDIOFILER_TRANSPORTSTRIP_H
#define AUDIOFILER_TRANSPORTSTRIP_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Components/TransportButton.h"
#include "Utils/Config.h"

/**
 * @file TransportStrip.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief UI component containing the main audio transport controls.
 */

/**
 * @class TransportStrip
 * @brief A modular UI component housing the primary audio transport controls (Play, Stop, Repeat, etc.).
 * 
 * @details Architecturally, the TransportStrip is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and is responsible only for rendering and organizing the transport 
 *          buttons. It relies entirely on its parent ControlPanel and specialized 
 *          presenters (like ControlButtonsPresenter and RepeatButtonPresenter) 
 *          to handle state updates and command routing. This decoupling ensures 
 *          that the transport UI remains purely visual and can be easily 
 *          re-themed or rearranged.
 * 
 * @see ControlButtonsPresenter, RepeatButtonPresenter, ControlPanel, TransportButton
 */
class TransportStrip : public juce::Component {
  public:
    /** @brief Constructs a new TransportStrip and initialises its buttons. */
    TransportStrip();

    /** @brief Destructor. */
    ~TransportStrip() override = default;

    /** @brief Standard JUCE resized callback, organizing the transport buttons. */
    void resized() override;

    /** 
     * @brief Updates the play button's visual state based on playback. 
     * @param isPlaying True if audio is currently playing.
     */
    void updatePlayButtonText(bool isPlaying);

    /** 
     * @brief Updates the visual state of the cut mode button. 
     * @param isCutModeActive True if cut mode is currently engaged.
     */
    void updateCutModeState(bool isCutModeActive);

    /** 
     * @brief Updates the visual state of the auto-play toggle button. 
     * @param isAutoplayActive True if auto-play is currently active.
     */
    void updateAutoplayState(bool isAutoplayActive);

    /** 
     * @brief Updates the visual state of the repeat/loop button. 
     * @param isRepeating True if loop playback is currently active.
     */
    void updateRepeatState(bool isRepeating);

    /** @return Reference to the combined play/stop toggle button. */
    TransportButton &getPlayStopButton() {
        return playStopButton;
    }
    /** @return Reference to the dedicated stop button. */
    TransportButton &getStopButton() {
        return stopButton;
    }
    /** @return Reference to the auto-play toggle button. */
    TransportButton &getAutoplayButton() {
        return autoplayButton;
    }
    /** @return Reference to the repeat/loop toggle button. */
    TransportButton &getRepeatButton() {
        return repeatButton;
    }
    /** @return Reference to the cut/commit action button. */
    TransportButton &getCutButton() {
        return cutButton;
    }

  private:
    TransportButton playStopButton;
    TransportButton stopButton;
    TransportButton autoplayButton;
    TransportButton repeatButton;
    TransportButton cutButton;

    /** @brief Configures the visual appearance and properties of the transport buttons. */
    void initialiseButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportStrip)
};

#endif
