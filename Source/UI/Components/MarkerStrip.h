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

/**
 * @file MarkerStrip.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Modular UI component for controlling cut points (In or Out).
 */

class BoundaryLogicPresenter;

/**
 * @class MarkerStrip
 * @brief A modular UI component for controlling a specific cut point (In or Out).
 * 
 * @details Architecturally, the MarkerStrip is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and is responsible only for rendering its constituent buttons 
 *          and editors according to the application's symmetry rules. It relies 
 *          entirely on its parent ControlPanel and injected Presenters (like 
 *          BoundaryLogicPresenter) to handle state changes and user interactions. 
 *          This decoupling ensures that the visual representation of marker controls 
 *          remains purely presentation-focused.
 * 
 *          Follows the Symmetry Rule:
 *          - In Strip: [In(L), Timer, Reset, Threshold, AutoCut(R)]
 *          - Out Strip: [AutoCut(L), Threshold, Reset, Timer, Out(R)]
 * 
 * @see BoundaryLogicPresenter, ControlPanel, TransportButton, StyledTextEditor
 */
class MarkerStrip : public juce::Component {
  public:
    /** 
     * @enum MarkerType
     * @brief Specifies whether this strip controls the 'In' or 'Out' point.
     */
    enum class MarkerType { 
        In,  /**< Configures the strip for the 'In' cut boundary. */
        Out  /**< Configures the strip for the 'Out' cut boundary. */
    };

    /**
     * @brief Constructs a new MarkerStrip.
     * @param type The role of this strip (In or Out).
     */
    explicit MarkerStrip(MarkerType type);

    /** @brief Destructor. */
    ~MarkerStrip() override = default;

    /** @brief Standard JUCE resized callback, organizing sub-components. */
    void resized() override;

    /** 
     * @brief Updates the timer text display. 
     * @param text The new time string to display.
     */
    void updateTimerText(const juce::String &text);

    /** 
     * @brief Updates the visual state of the auto-cut button. 
     * @param isActive True if auto-cut is currently active.
     */
    void updateAutoCutState(bool isActive);

    /** 
     * @brief Changes the color of the primary marker button. 
     * @param color The new color to apply.
     */
    void updateMarkerButtonColor(juce::Colour color);

    /** 
     * @brief Injects the logic presenter for this strip. 
     * @param p Pointer to the boundary logic presenter.
     */
    void setPresenter(BoundaryLogicPresenter *p) {
        boundaryLogicPresenter = p;
    }

    /** @return Reference to the main marker button. */
    TransportButton &getMarkerButton() {
        return markerButton;
    }
    /** @return Reference to the time-based text editor. */
    StyledTextEditor &getTimerEditor() {
        return timerEditor;
    }
    /** @return Reference to the boundary reset button. */
    TransportButton &getResetButton() {
        return resetButton;
    }
    /** @return Reference to the silence threshold text editor. */
    StyledTextEditor &getThresholdEditor() {
        return thresholdEditor;
    }
    /** @return Reference to the auto-cut toggle button. */
    TransportButton &getAutoCutButton() {
        return autoCutButton;
    }
    /** @return Reference to the boundary lock button. */
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

    /** @brief Initialises and configures sub-components based on MarkerType. */
    void initialiseComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerStrip)
};

#endif
