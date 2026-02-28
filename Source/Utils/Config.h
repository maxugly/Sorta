#ifndef AUDIOFILER_CONFIG_H
#define AUDIOFILER_CONFIG_H

#if !defined(JUCE_HEADLESS)
#include <juce_graphics/juce_graphics.h>
#else
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#endif

/**
 * @file Config.h
 * @ingroup Helpers
 * @namespace Config
 * @brief The central configuration hub for the Audiofiler application.
 * 
 * @details Architecturally, the Config namespace acts as a "Static Parameter Store" that 
 *          centralizes all magic numbers, color palettes, and layout constants. By 
 *          removing these values from the logic and view classes, we achieve a 
 *          clean separation between "how things work" and "how things look."
 * 
 *          The namespace is organized into sub-namespaces:
 *          - **UI**: Geometric units and spacing constants.
 *          - **Colors**: The global theme palette (Window, Button, Waveform, etc.).
 *          - **Layout**: Dynamic and static sizing for windows and components.
 *          - **Animation**: Constants driving transition speeds and highlight intensities.
 *          - **Audio**: Fallback sample rates and buffer sizes.
 *          - **Labels**: Internationalizable string constants.
 * 
 * @see ModernLookAndFeel
 * @see SessionState
 */
namespace Config {

/**
 * @brief Initializes all dynamic configuration values to their factory defaults.
 * @details This must be called early in the application lifecycle, typically within 
 *          `MainComponent`'s constructor.
 */
void initializeConfigs();

/**
 * @brief Loads a visual theme from an external JSON or binary file.
 * @param themeFile The file handle to the theme definition.
 */
void loadTheme(const juce::File& themeFile);

/**
 * @brief Serializes the current color palette to a named theme file.
 * @param themeName The identifier for the theme.
 */
void saveCurrentTheme(const juce::String& themeName);

/** @brief UI-specific geometric constants. */
namespace UI {
    /** @brief The base thickness of the button border. */
    inline constexpr float ButtonOutlineThickness = 2.0f;
    /** @brief Standard corner rounding for all buttons. */
    inline constexpr float ButtonCornerSize = 6.0f;
    /** @brief Width for Cut In/Out buttons in units. */
    inline constexpr float CutButtonWidthUnits = 2.5f;
    /** @brief Width for Time Editors (Timers) in units. (220px / 32.0) */
    inline constexpr float TimerWidthUnits = 6.875f;
    /** @brief Width for Reset buttons in units. */
    inline constexpr float ResetButtonWidthUnits = 1.5f;
    /** @brief Width for Threshold editors in units. */
    inline constexpr float ThresholdWidthUnits = 1.5f;
    /** @brief The base unit for widgets (all dimensions are multiples of this). */
    inline constexpr float WidgetUnit = 32.0f;
    /** @brief The standard height for widgets. */
    inline constexpr float WidgetHeight = WidgetUnit;
    /** @brief The spacing between grouped buttons. */
    inline constexpr float GroupSpacing = 1.0f;
} // namespace UI

/** @brief Global color palette for the application. */
namespace Colors {
    struct Window {
        static juce::Colour background; /**< Main application background. */
    };

    extern juce::Colour transparentBlack; /**< Helper for alpha-blended shadows. */
    extern juce::Colour solidBlack;       /**< Absolute black. */
    extern juce::Colour transparentWhite; /**< Helper for alpha-blended highlights. */

    struct Button {
        static juce::Colour base;               /**< Default button fill. */
        static juce::Colour on;                 /**< Active/Toggled button fill. */
        static juce::Colour text;               /**< Label color. */
        static juce::Colour outline;            /**< Border color. */
        static juce::Colour disabledBackground; /**< Fill for inactive buttons. */
        static juce::Colour disabledText;       /**< Label for inactive buttons. */
        static juce::Colour exit;               /**< High-visibility exit button. */
        static juce::Colour exitText;           /**< Exit button label. */
        static juce::Colour clear;              /**< Secondary action color. */
        static juce::Colour cutPlacement;       /**< Highlight for marker placement. */
        static juce::Colour cutActive;          /**< State color for active cuts. */
    };

    extern juce::Colour playbackText;        /**< Color for the primary playhead timer. */
    extern juce::Colour playbackCursorHead;  /**< Top/Bottom marker color for the playhead. */
    extern juce::Colour cutText;             /**< Color for cut boundary timers. */
    extern juce::Colour totalTimeText;       /**< Color for total duration labels. */
    extern juce::Colour textEditorBackground; /**< Field fill for text inputs. */
    extern juce::Colour textEditorError;     /**< Border/Background for invalid inputs. */
    extern juce::Colour textEditorWarning;   /**< Border for near-limit inputs. */
    extern juce::Colour textEditorOutOfRange; /**< Highlight for clipped values. */
    extern juce::Colour waveformPeak;        /**< Outer peak color of the waveform. */
    extern juce::Colour waveformCore;        /**< Inner core color of the waveform. */
    extern juce::Colour playbackCursor;      /**< The primary playhead line. */
    extern juce::Colour cutRegion;           /**< Shaded overlay for the cut zone. */
    extern juce::Colour cutLine;             /**< Boundary marker line. */
    extern juce::Colour cutMarkerAuto;       /**< Color for markers set by the auto-engine. */
    extern juce::Colour cutMarkerHover;      /**< Highlight color when mouse is over a marker. */
    extern juce::Colour cutMarkerDrag;       /**< Active color during marker manipulation. */
    extern juce::Colour fpsBackground;       /**< FPS counter backdrop. */
    extern juce::Colour fpsText;             /**< FPS counter digits. */
    extern juce::Colour mouseCursorLine;     /**< Vertical guide following the mouse. */
    extern juce::Colour mouseCursorHighlight; /**< Visual cue for the mouse position. */
    extern juce::Colour mouseAmplitudeLine;  /**< Horizontal amplitude guide. */
    extern juce::Colour mousePlacementMode;  /**< Visual cue for active placement state. */
    extern juce::Colour thresholdLine;       /**< Silence threshold indicator. */
    extern juce::Colour thresholdRegion;     /**< Shaded area for the silence zone. */
    extern juce::Colour statsBackground;     /**< Backdrop for the metadata panel. */
    extern juce::Colour statsText;           /**< Text in the metadata panel. */
    extern juce::Colour statsErrorText;      /**< Error text in the metadata panel. */
    extern juce::Colour mouseAmplitudeGlow;  /**< Bloom effect for amplitude guides. */
    extern juce::Colour placementModeGlow;   /**< Bloom effect for placement guides. */
    extern juce::Colour zoomPopupBorder;     /**< Outline for the zoom preview window. */
    extern juce::Colour zoomPopupTrackingLine; /**< Guide line in the zoom preview. */
    extern juce::Colour zoomPopupPlaybackLine; /**< Playhead line in the zoom preview. */
    extern juce::Colour zoomPopupZeroLine;    /**< Center line in the zoom preview. */
    extern juce::Colour volumeKnobFill;      /**< Active track of the volume dial. */
    extern juce::Colour volumeKnobTrack;     /**< Inactive track of the volume dial. */
    extern juce::Colour volumeKnobPointer;   /**< Needle color for the volume dial. */
    extern juce::Colour quaternary;          /**< Secondary auxiliary color. */

    struct ZoomHud {
        static juce::Colour background;   /**< Backdrop for the zoom info display. */
        static juce::Colour textActive;   /**< Active focus text. */
        static juce::Colour textInactive; /**< Secondary info text. */
    };

    struct VolumeFlame {
        static juce::Colour low;  /**< Low intensity meter color. */
        static juce::Colour mid;  /**< Mid intensity meter color. */
        static juce::Colour high; /**< High intensity meter color. */
        static juce::Colour peak; /**< Peak/Clipped meter color. */
    };

    struct HintVox {
        static juce::Colour text; /**< Color for the status/hint text. */
    };

    struct Matrix {
        static juce::Colour ledActive;   /**< Active LED indicator color. */
        static juce::Colour ledInactive; /**< Inactive LED indicator color. */
    };
} // namespace Colors

/** @brief Component sizing and positioning constants. */
struct Layout {
    struct Window {
        static int width;  /**< Default startup width. */
        static int height; /**< Default startup height. */
    };
    static constexpr int windowBorderMargins = 15; /**< Internal padding for main layout. */
    static int buttonHeight;                       /**< Standard button height. */
    static int buttonWidth;                        /**< Standard button width. */
    static int clearButtonWidth;                   /**< Width for secondary actions. */
    static float buttonCornerRadius;               /**< Corner rounding factor. */
    static constexpr float buttonOutlineThickness = 1.0f;
    static constexpr float connectorLineWidth = 2.0f;
    static constexpr float outlineThicknessMedium = 2.5f;
    static constexpr int cutTextWidth = 165;
    static constexpr int thresholdEditorWidth = 80;
    static constexpr int cutLengthStripWidth = 268;

    struct VolumeKnob {
        static constexpr float outlineThickness = 2.0f;
        static constexpr float pointerThickness = 3.0f;
        static constexpr float pointerLengthProportion = 0.4f;
    };

    struct Text {
        static constexpr int playbackWidth = 220;
        static constexpr int playbackHeight = 30;
        static constexpr int playbackOffsetY = 25;
        static constexpr int playbackSize = 30;
        static constexpr int mouseCursorSize = 20;
        static constexpr float buttonHeightScale = 0.45f;
        static constexpr float buttonPlayPauseHeightScale = 0.7f;
        static constexpr float backgroundAlpha = 0.7f;
        static constexpr int editorOutlineThickness = 1;
    };

    struct Stats {
        static constexpr int initialHeight = 150;
        static constexpr int minHeight = 50;
        static constexpr int maxHeight = 600;
        static constexpr float cornerRadius = 4.0f;
        static constexpr int handleAreaHeight = 12;
        static constexpr int handleWidth = 40;
        static constexpr int handleLineHeight = 2;
        static constexpr float handleAlpha = 0.3f;
        static constexpr int internalPadding = 2;
        static constexpr int sideMargin = 10;
        static constexpr int topMargin = 10;
    };

    struct Waveform {
        static float heightScale;             /**< Proportion of window occupied by waveform. */
        static int pixelsPerSampleLow;       /**< Zoom resolution (Low). */
        static int pixelsPerSampleMedium;    /**< Zoom resolution (Medium). */
        static int pixelsPerSampleHigh;      /**< Zoom resolution (High). */
    };

    struct Glow {
        static constexpr float offsetFactor = 0.5f;
        static constexpr float mouseAlpha = 0.3f;
        static constexpr float hitBoxTolerance = 20.0f; /**< Precision for mouse-marker interaction. */
        static constexpr int mousePadding = 2;
        static constexpr int mouseHighlightOffset = 2;
        static constexpr int mouseHighlightSize = 5;
        static constexpr float mouseAmplitudeAlpha = 0.7f;
        static constexpr int glowRadius = 15;
        static constexpr int mouseTextOffset = 5;
        static float thickness;
        static constexpr float mouseAmplitudeGlowThickness = 3.0f;
        static constexpr float placementModeGlowThickness = 3.0f;
        static constexpr float thresholdGlowThickness = 3.0f;
        static float cutLineGlowThickness;
        static float cutMarkerWidthThin;
        static constexpr float cutBoxOutlineThickness = 5.5f;
        static constexpr float cutBoxOutlineThicknessInteracting = 10.0f;
        static constexpr float cutMarkerBoxWidth = 30.0f;
        static constexpr int cutMarkerBoxHeight = 30;
        static constexpr float cutMarkerCenterDivisor = 2.0f;
    };

    struct Zoom {
        static constexpr float popupScale = 0.8f;
        static constexpr float borderThickness = 3.0f;
        static constexpr int hudPadding = 10;
        static constexpr int hudFontSize = 18;
        static constexpr int hudLineSpacing = 22;
        static constexpr float zoomStepIn = 1.1f;
        static constexpr float zoomStepOut = 0.9f;
    };

    struct Matrix {
        static constexpr int squareSize = 6;
        static constexpr int rows = 4;
        static constexpr int columns = 16;
        static constexpr int padding = 8;
        static constexpr int startX = 4;
    };

    struct TopBar {
        static constexpr int volumeKnobSize = 80;
        static constexpr int themeSelectorWidth = 120;
        static constexpr int hintWidth = 150;
        static constexpr int transportButtonsCount = 5;
        static constexpr int transportButtonsSpacingCount = 4;
    };

    struct Fps {
        static constexpr int width = 125;
        static constexpr int height = 24;
        static constexpr int margin = 10;
        static constexpr float cornerRadius = 4.0f;
    };
};

/** @brief Timing and behavior constants for visual feedback. */
namespace Animation {
    constexpr float buttonHighlightedBrightness = 0.1f;
    constexpr float buttonPressedDarkness = 0.1f;
    constexpr float mouseAmplitudeLineLength = 50.0f;
    constexpr float thresholdLineWidth = 100.0f;
} // namespace Animation

/** @brief Default technical parameters for the audio engine. */
namespace Audio {
    constexpr int thumbnailCacheSize = 5;
    constexpr int thumbnailSizePixels = 512;
    static constexpr double fallbackSampleRate = 44100.0;
    constexpr double keyboardSkipSeconds = 5.0;
    constexpr double cutStepHours = 3600.0;
    constexpr double cutStepMinutes = 60.0;
    constexpr double cutStepSeconds = 1.0;
    constexpr double cutStepMilliseconds = 0.01;
    constexpr double cutStepMillisecondsFine = 0.001;
    constexpr int readAheadBufferSize = 32768;
    constexpr float silenceThresholdIn = 0.01f;
    constexpr float silenceThresholdOut = 0.01f;
    constexpr bool lockHandlesWhenAutoCutActive = false;
} // namespace Audio

/** @brief User preferences and experimental features. */
namespace Advanced {
    extern bool showFpsOverlay;
    extern int fpsOverlayX;
    extern int fpsOverlayY;
    extern juce::String fpsOverlayPosition;
    extern juce::String currentTheme;

    extern juce::String posTopCenter;
    extern juce::String posBottomCenter;
} // namespace Advanced

/** @brief Global string table for localization and UI consistency. */
namespace Labels {
    extern juce::String appName;
    extern juce::String appVersion;
    extern juce::String defaultHint;
    extern juce::String openButton;
    extern juce::String selectAudio;
    extern juce::String timeSeparator;
    extern juce::String openBracket;
    extern juce::String closeBracket;
    extern juce::String playButton;
    extern juce::String stopButton;
    extern juce::String viewModeClassic;
    extern juce::String viewModeOverlay;
    extern juce::String channelViewMono;
    extern juce::String channelViewStereo;
    extern juce::String exitButton;
    extern juce::String statsButton;
    extern juce::String repeatButton;
    extern juce::String cutInButton;
    extern juce::String cutOutButton;
    extern juce::String clearButton;
    extern juce::String autoplayButton;
    extern juce::String autoCutInButton;
    extern juce::String autoCutOutButton;
    extern juce::String cutButton;
    extern juce::String themeUp;
    extern juce::String themeDown;
    extern juce::String lockLocked;
    extern juce::String lockUnlocked;
    extern juce::String silenceThresholdInTooltip;
    extern juce::String silenceThresholdOutTooltip;
    extern juce::String zoomPrefix;
    extern juce::String stepDefault;
    extern juce::String stepShift;
    extern juce::String stepAlt;
    extern juce::String stepCtrlShift;
    extern juce::String statsFile;
    extern juce::String statsSamples;
    extern juce::String statsRate;
    extern juce::String statsHz;
    extern juce::String statsChannels;
    extern juce::String statsLength;
    extern juce::String statsPeak0;
    extern juce::String statsPeak1;
    extern juce::String statsMin;
    extern juce::String statsMax;
    extern juce::String statsError;
    extern juce::String logNoAudio;
    extern juce::String logScanning;
    extern juce::String logSamplesFor;
    extern juce::String logBoundary;
    extern juce::String logZeroLength;
    extern juce::String logStartSet;
    extern juce::String logEndSet;
    extern juce::String logNoSound;
    extern juce::String logTooLarge;
    extern juce::String errorZeroLength;
    extern juce::String errorNoAudio;
    extern juce::String scanningCutPoints;
    extern juce::String noSilenceBoundaries;
    extern juce::String hintViewPrefix;
    extern juce::String hintViewClassic;
    extern juce::String hintViewOverlay;
    extern juce::String hintChannelsPrefix;
    extern juce::String hintChannelsStereo;
    extern juce::String hintChannelsMono;
    extern juce::String selectTheme;
    extern juce::String fpsSuffix;
    extern juce::String fpsClose;
    extern juce::String timePrefixPlus;
    extern const char* const threadAudioReader;
    extern const char* const failGeneric;
} // namespace Labels

} // namespace Config

#endif
