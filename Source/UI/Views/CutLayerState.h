#ifndef AUDIOFILER_CUTLAYERSTATE_H
#define AUDIOFILER_CUTLAYERSTATE_H

#include <JuceHeader.h>

#include "Core/AppEnums.h"
#include "Utils/Config.h"

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

#endif
