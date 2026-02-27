#ifndef AUDIOFILER_MARKERSTRIP_H
#define AUDIOFILER_MARKERSTRIP_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Components/TransportButton.h"
#include "UI/Components/StyledTextEditor.h"
#include "Utils/Config.h"

class BoundaryLogicPresenter;

/**
 * @ingroup UI
 * @class MarkerStrip
 * @brief A modular component for controlling a cut point (In or Out).
 * @details Follows the Symmetry Rule:
 *          In Strip: [In(L), Timer, Reset, Threshold, AutoCut(R)]
 *          Out Strip: [AutoCut(L), Threshold, Reset, Timer, Out(R)]
 */
class MarkerStrip : public juce::Component {
  public:
    enum class MarkerType { In, Out };

    explicit MarkerStrip(MarkerType type);
    ~MarkerStrip() override = default;

    void resized() override;

    void updateTimerText(const juce::String &text);
    void updateAutoCutState(bool isActive);
    void updateMarkerButtonColor(juce::Colour color);

    void setPresenter(BoundaryLogicPresenter *p) {
        boundaryLogicPresenter = p;
    }

    TransportButton &getMarkerButton() {
        return markerButton;
    }
    StyledTextEditor &getTimerEditor() {
        return timerEditor;
    }
    TransportButton &getResetButton() {
        return resetButton;
    }
    StyledTextEditor &getThresholdEditor() {
        return thresholdEditor;
    }
    TransportButton &getAutoCutButton() {
        return autoCutButton;
    }
    TransportButton &getLockButton() {
        return lockButton;
    }

  private:
    MarkerType markerType;
    BoundaryLogicPresenter *boundaryLogicPresenter = nullptr;

    TransportButton markerButton;
    StyledTextEditor timerEditor;
    TransportButton resetButton;
    StyledTextEditor thresholdEditor;
    TransportButton autoCutButton;
    TransportButton lockButton;

    void initialiseComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerStrip)
};

#endif
