#pragma once

/*
 * This file contains configurable settings for the Sorta application.
 * It's a central place for tweaking the look and feel and other
 * parameters without having to dig through the source code.
 */

namespace Config{
    // The margin around the entire window content, in pixels.
    constexpr int windowBorderMargins = 15;

    // The initial width of the main window in pixels.
    constexpr int initialWindowWidth = 1200;

    // The initial height of the main window in pixels.
    constexpr int initialWindowHeight = 800;

    // Playback text settings
    const juce::Colour playbackTextColor = juce::Colour(0xFF34FA11); // Changed to const as juce::Colour constructor is not constexpr
    constexpr float playbackTextBackgroundAlpha = 0.7f; // 0.0f (fully transparent) to 1.0f (fully opaque)
    constexpr int playbackTextSize = 30;

    // Playback and Loop text widths
    constexpr int playbackTextWidth = 220;
    constexpr int loopTextWidth = 165;

    // --- Button Settings ---
    constexpr int buttonWidth = 80;
    constexpr int buttonHeight = 30; // Will be used in resized()
    constexpr float buttonCornerRadius = 5.0f;
    constexpr float buttonOutlineThickness = 1.0f;
    const juce::Colour buttonOutlineColour = juce::Colour(0xff808080); // Medium Grey Outline

    const juce::Colour buttonBaseColour = juce::Colour(0xff3a3a3a); // Dark Grey
    const juce::Colour buttonOnColour = juce::Colour(0xff00bfff);   // Deep Sky Blue (for toggled/highlighted state)
    const juce::Colour buttonTextColour = juce::Colour(0xFFFFFFFF); // White
    constexpr float buttonHighlightedBrightnessFactor = 0.1f; // How much brighter on hover
    constexpr float buttonPressedDarknessFactor = 0.1f;   // How much darker on press

    constexpr float buttonTextHeightScale = 0.45f;         // Default scale for button text
    constexpr float buttonPlayPauseTextHeightScale = 0.7f; // Scale for Play/Pause symbols
} // namespace Config
