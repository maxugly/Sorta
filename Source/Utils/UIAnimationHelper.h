#ifndef AUDIOFILER_UIANIMATIONHELPER_H
#define AUDIOFILER_UIANIMATIONHELPER_H

#include <cmath>

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file UIAnimationHelper.h
 * @ingroup UI
 * @brief Header-only utility for normalized, periodic animation curves.
 * 
 * @details Architecturally, UIAnimationHelper is a "Pure Function Provider" 
 *          that transforms a linear master phase into visually pleasing, 
 *          periodic curves (e.g., sine pulses). By abstracting the 
 *          trigonometric logic here, we ensure that UI components (like 
 *          `PlaybackCursorGlow` or `TransportButton`) can implement 
 *          sophisticated pulsing and highlighting without cluttering 
 *          their `paint()` methods with complex math.
 * 
 *          The class follows a "Deterministic Animation" pattern—given the 
 *          same master phase and multiplier, it will always return the 
 *          exact same value, making it highly testable and resource-efficient.
 */
class UIAnimationHelper {
  public:
    /**
     * @brief Transforms a linear clock phase into a periodic sine pulse.
     * @details Mathematical approach: 
     *          1. Calculate `sin(phase * multiplier * 2π)`.
     *          2. Map the resulting range `[-1.0, 1.0]` to `[0.0, 1.0]`.
     * 
     * @param masterPhase A linear value from 0.0 to 1.0 representing 
     *                    the current global clock state.
     * @param multiplier The frequency factor (e.g., 2.0 doubles the pulse speed).
     * @return A float value between 0.0 and 1.0 representing the pulse intensity.
     */
    static float getSinePulse(float masterPhase, float multiplier) {
        float rawSine = std::sin(masterPhase * multiplier * juce::MathConstants<float>::twoPi);
        // Map [-1.0, 1.0] to [0.0, 1.0]
        return (rawSine + 1.0f) * 0.5f;
    }
};

#endif
