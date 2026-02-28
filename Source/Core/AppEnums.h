#ifndef AUDIOFILER_APPENUMS_H
#define AUDIOFILER_APPENUMS_H

/**
 * @file AppEnums.h
 * @ingroup State
 * @brief Global enumerations and constants for the Audiofiler application.
 * 
 * @details This file defines the core symbolic constants used to drive UI logic, 
 *          view layouts, and state machine transitions. By centralizing these 
 *          enums in the State module, we ensure consistent terminology and 
 *          behavior across all Presenters and Views.
 */
namespace AppEnums {

/**
 * @brief Defines the visual layout and interaction style of the primary UI.
 */
enum class ViewMode { 
    Classic, /**< Standard layout with discrete panels and sidebar controls. */
    Overlay  /**< Minimalist, transparent layout focusing on the waveform canvas. */
};

/**
 * @brief Indicates which boundary marker is currently being manipulated or highlighted.
 */
enum class PlacementMode { 
    None,   /**< No marker is currently selected for movement. */
    CutIn,  /**< The 'In' boundary marker is active. */
    CutOut  /**< The 'Out' boundary marker is active. */
};

/**
 * @brief Controls the rendering strategy for multi-channel audio files.
 */
enum class ChannelViewMode { 
    Mono,   /**< Sums channels or displays only the first channel for simplicity. */
    Stereo  /**< Renders both Left and Right channels independently. */
};

/**
 * @brief Specifies the focal point for horizontal zooming operations.
 */
enum class ActiveZoomPoint { 
    None,     /**< Zoom is centered around the middle of the current view. */
    In,       /**< Zoom follows the 'In' marker. */
    Out,      /**< Zoom follows the 'Out' marker. */
    Playback  /**< Zoom follows the active playback cursor. */
};

/**
 * @brief Used for styling UI components that are part of a horizontal button group.
 * @details This enum allows the ModernLookAndFeel to apply specific corner rounding 
 *          to buttons based on their position in a cluster.
 */
enum class GroupPosition { 
    Alone,  /**< A single, standalone button with full corner rounding. */
    Left,   /**< The leftmost button in a group; only left corners rounded. */
    Middle, /**< A middle button with no corner rounding. */
    Right   /**< The rightmost button in a group; only right corners rounded. */
};
} // namespace AppEnums

#endif
