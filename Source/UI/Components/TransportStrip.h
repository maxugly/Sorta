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
 * @ingroup UI
 * @class TransportStrip
 * @brief A modular component containing the main transport controls.
 * @details Owns Play, Stop, Auto-Play, Repeat, and Cut buttons.
 *          Handles its own layout and callbacks.
 */
class TransportStrip : public juce::Component {
  public:
    TransportStrip();
    ~TransportStrip() override = default;

    void resized() override;

    void updatePlayButtonText(bool isPlaying);
    void updateCutModeState(bool isCutModeActive);
    void updateAutoplayState(bool isAutoplayActive);
    void updateRepeatState(bool isRepeating);

    TransportButton &getPlayStopButton() {
        return playStopButton;
    }
    TransportButton &getStopButton() {
        return stopButton;
    }
    TransportButton &getAutoplayButton() {
        return autoplayButton;
    }
    TransportButton &getRepeatButton() {
        return repeatButton;
    }
    TransportButton &getCutButton() {
        return cutButton;
    }

  private:
    TransportButton playStopButton;
    TransportButton stopButton;
    TransportButton autoplayButton;
    TransportButton repeatButton;
    TransportButton cutButton;

    void initialiseButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportStrip)
};

#endif
