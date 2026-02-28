#ifndef AUDIOFILER_COORDINATEMAPPER_H
#define AUDIOFILER_COORDINATEMAPPER_H

#include <JuceHeader.h>

/**
 * @file CoordinateMapper.h
 * @ingroup Helpers
 * @brief Mathematical utility for translating between time and pixel domains.
 * 
 * @details Architecturally, CoordinateMapper serves as a "Mathematical Bridge" between 
 *          the temporal audio domain (seconds) and the spatial UI domain (pixels). 
 *          It provides normalized, scale-independent calculations that allow 
 *          waveform views and marker handlers to remain decoupled from the 
 *          specific dimensions of the parent container.
 * 
 *          By centralizing these calculations, we ensure that the playback cursor, 
 *          cut boundaries, and mouse interactions always remain perfectly synchronized 
 *          regardless of window resizing or display scaling.
 */
class CoordinateMapper {
  public:
    /**
     * @brief Translates a horizontal pixel position to a time value in seconds.
     * @details This performs a proportional mapping: (x / width) * duration.
     * @param x The horizontal pixel coordinate (local to the component).
     * @param componentWidth The total width of the rendering area in pixels.
     * @param totalDuration The total length of the audio file in seconds.
     * @return The corresponding time in seconds, or 0.0 if parameters are invalid.
     */
    static double pixelsToSeconds(float x, float componentWidth, double totalDuration) {
        if (componentWidth <= 0.0f || totalDuration <= 0.0)
            return 0.0;

        return (static_cast<double>(x) / static_cast<double>(componentWidth)) * totalDuration;
    }

    /**
     * @brief Translates a time value in seconds to a horizontal pixel coordinate.
     * @details This performs a proportional mapping: (seconds / duration) * width.
     * @param seconds The time position to translate.
     * @param componentWidth The total width of the rendering area in pixels.
     * @param totalDuration The total length of the audio file in seconds.
     * @return The corresponding horizontal pixel coordinate, or 0.0f if parameters are invalid.
     */
    static float secondsToPixels(double seconds, float componentWidth, double totalDuration) {
        if (totalDuration <= 0.0 || componentWidth <= 0.0f)
            return 0.0f;

        return static_cast<float>((seconds / totalDuration) * static_cast<double>(componentWidth));
    }
};

#endif
