#ifndef AUDIOFILER_PLAYBACKHELPERS_H
#define AUDIOFILER_PLAYBACKHELPERS_H

#include <juce_core/juce_core.h>

/**
 * @file PlaybackHelpers.h
 * @ingroup Helpers
 * @brief Static utility for enforcing playback constraints and temporal logic.
 * 
 * @details Architecturally, PlaybackHelpers serves as a "Validation Provider" 
 *          that encapsulates the core boundary logic used by the AudioPlayer and 
 *          transport-related Presenters. By centralizing these calculations, 
 *          we ensure that "Cut Mode" behavior remains consistent across the 
 *          application, whether the playhead is being moved via mouse, keyboard, 
 *          or automatic playback.
 */
class PlaybackHelpers {
  public:
    /**
     * @brief Clamps a target playback position within the active 'In' and 'Out' markers.
     * @param position The requested position in seconds.
     * @param cutIn The current 'In' marker boundary in seconds.
     * @param cutOut The current 'Out' marker boundary in seconds.
     * @return The constrained position, clamped strictly between cutIn and cutOut.
     */
    static double constrainPosition(double position, double cutIn, double cutOut);
};

#endif
