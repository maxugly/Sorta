#ifndef AUDIOFILER_THEMELOADER_H
#define AUDIOFILER_THEMELOADER_H

#include "Utils/Config.h"

/**
 * @file ThemeLoader.h
 * @ingroup Helpers
 * @brief Utility for theme file IO and JSON parsing.
 */
class ThemeLoader {
  public:
    /**
     * @brief Loads a visual theme from an external JSON or binary file.
     * @param themeFile The file handle to the theme definition.
     */
    static void loadTheme(const juce::File& themeFile);

    /**
     * @brief Generates a default theme file if none exists.
     * @param themeFile The file handle to the theme definition.
     */
    static void generateDefaultTheme(const juce::File& themeFile);
};

#endif
