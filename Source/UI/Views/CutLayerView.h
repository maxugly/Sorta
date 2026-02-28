#ifndef AUDIOFILER_CUTLAYERVIEW_H
#define AUDIOFILER_CUTLAYERVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Utils/Config.h"
#include "UI/Handlers/MarkerMouseHandler.h"

/**
 * @file CutLayerView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief View component responsible for rendering cut markers, thresholds, and regions.
 */

class ControlPanel;

/**
 * @struct CutLayerState
 * @brief Encapsulates the complete visual state required to render the cut layer overlay.
 * 
 * @details This struct acts as a "Passive Data Model" for the CutLayerView. It 
 *          contains only visual properties (coordinates, colors, flags) and 
 *          no logic, allowing the presenter to calculate and push the exact 
 *          state needed for rendering.
 */
struct CutLayerState {
    /** @brief The pixel X coordinate for the 'In' marker. */
    float inPixelX{0.0f};
    /** @brief The pixel X coordinate for the 'Out' marker. */
    float outPixelX{0.0f};
    /** @brief The actual X coordinate for the 'In' marker (accounting for bounds). */
    float actualInX{0.0f};
    /** @brief The actual X coordinate for the 'Out' marker (accounting for bounds). */
    float actualOutX{0.0f};
    /** @brief The vertical pixel Y coordinate for the top of the 'In' threshold line. */
    float inThresholdYTop{0.0f};
    /** @brief The vertical pixel Y coordinate for the bottom of the 'In' threshold line. */
    float inThresholdYBottom{0.0f};
    /** @brief The vertical pixel Y coordinate for the top of the 'Out' threshold line. */
    float outThresholdYTop{0.0f};
    /** @brief The vertical pixel Y coordinate for the bottom of the 'Out' threshold line. */
    float outThresholdYBottom{0.0f};
    /** @brief The total duration of the audio in seconds. */
    float audioLength{0.0};
    /** @brief Flag indicating if markers should be drawn. */
    bool markersVisible{false};
    /** @brief The current channel mode (Mono/Stereo). */
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};

    /** @brief The color of the 'In' marker. */
    juce::Colour inMarkerColor;
    /** @brief The thickness of the 'In' marker line in pixels. */
    float inMarkerThickness{0.0f};
    /** @brief True if the 'In' marker should currently be pulsing. */
    bool inMarkerShouldPulse{false};

    /** @brief The color of the 'Out' marker. */
    juce::Colour outMarkerColor;
    /** @brief The thickness of the 'Out' marker line in pixels. */
    float outMarkerThickness{0.0f};
    /** @brief True if the 'Out' marker should currently be pulsing. */
    bool outMarkerShouldPulse{false};

    /** @brief The color of the 'In' threshold line. */
    juce::Colour inThresholdColor;
    /** @brief The color of the 'Out' threshold line. */
    juce::Colour outThresholdColor;

    /** @brief The outline color of the selected cut region. */
    juce::Colour regionOutlineColor;
    /** @brief The thickness of the region outline in pixels. */
    float regionOutlineThickness{0.0f};
    /** @brief True if the entire region should currently be pulsing. */
    bool regionShouldPulse{false};
};

/**
 * @class CutLayerView
 * @brief A passive UI component that renders the cut markers, silence thresholds, and selected regions.
 * 
 * @details Architecturally, the CutLayerView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the visual elements associated 
 *          with audio cutting (markers, thresholds, region highlights). It 
 *          relies entirely on the CutPresenter to calculate its visual state 
 *          (via CutLayerState) and push it through the updateState() method.
 * 
 * @see CutPresenter, WaveformCanvasView, CutLayerState
 */
class CutLayerView : public juce::Component {
  public:
    /**
     * @brief Constructs a new CutLayerView.
     * @param owner Reference to the parent ControlPanel.
     */
    explicit CutLayerView(ControlPanel &owner);

    /** @brief Destructor. */
    ~CutLayerView() override;

    /** 
     * @brief Updates the view's internal state and triggers a repaint. 
     * @param newState The new visual state to apply.
     */
    void updateState(const CutLayerState& newState);

    /** @return Const reference to the current visual state. */
    const CutLayerState& getCurrentState() const { return state; }

    /** @return Reference to the parent ControlPanel. */
    ControlPanel& getOwner() { return owner; }

    /** @brief Standard JUCE paint callback, rendering all layer elements. */
    void paint(juce::Graphics &g) override;

  private:
    /** @brief Renders the horizontal silence threshold lines. */
    void drawThresholds(juce::Graphics& g);
    /** @brief Renders the fade/cut regions (darkened areas). */
    void drawFadeRegions(juce::Graphics& g);
    /** @brief Renders the vertical marker lines and the selected region highlight. */
    void drawMarkersAndRegion(juce::Graphics& g);

    ControlPanel &owner;
    CutLayerState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLayerView)
};

#endif
