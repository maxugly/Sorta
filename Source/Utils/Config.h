#ifndef AUDIOFILER_CONFIG_H
#define AUDIOFILER_CONFIG_H

#if !defined(JUCE_HEADLESS)
#include <juce_graphics/juce_graphics.h>
#else
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#endif

/**
 * @ingroup State
 * @namespace Config
 * @brief Global configuration constants and settings.
 * @details This namespace contains static configuration for colors, layout, animation,
 *          and audio settings. It provides a centralized place to tweak the application's
 *          look and feel and behavior.
 */
namespace Config {

void initializeConfigs();
void loadTheme(const juce::File& themeFile);

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
/** @brief The base unit for widgets. */
inline constexpr float WidgetUnit = 32.0f;
/** @brief The standard height for widgets. */
inline constexpr float WidgetHeight = WidgetUnit;
/** @brief The spacing between grouped buttons. */
inline constexpr float GroupSpacing = 1.0f;
} // namespace UI

namespace Colors {
struct Window {
    static juce::Colour background;
};

extern juce::Colour transparentBlack;
extern juce::Colour solidBlack;
extern juce::Colour transparentWhite;

struct Button {
    static juce::Colour base;
    static juce::Colour on;
    static juce::Colour text;
    static juce::Colour outline;
    static juce::Colour disabledBackground;
    static juce::Colour disabledText;
    static juce::Colour exit;
    static juce::Colour clear;
    static juce::Colour cutPlacement;
    static juce::Colour cutActive;
};
extern juce::Colour playbackText;
extern juce::Colour cutText;
extern juce::Colour totalTimeText;
extern juce::Colour textEditorBackground;
extern juce::Colour textEditorError;
extern juce::Colour textEditorWarning;
extern juce::Colour textEditorOutOfRange;
extern juce::Colour waveformPeak;
extern juce::Colour waveformCore;
extern juce::Colour playbackCursor;
extern juce::Colour cutRegion;
extern juce::Colour cutLine;
extern juce::Colour cutMarkerAuto;
extern juce::Colour cutMarkerHover;
extern juce::Colour cutMarkerDrag;
extern juce::Colour fpsBackground;
extern juce::Colour fpsText;
extern juce::Colour mouseCursorLine;
extern juce::Colour mouseCursorHighlight;
extern juce::Colour mouseAmplitudeLine;
extern juce::Colour mousePlacementMode;
extern juce::Colour thresholdLine;
extern juce::Colour thresholdRegion;
extern juce::Colour statsBackground;
extern juce::Colour statsText;
extern juce::Colour statsErrorText;
extern juce::Colour mouseAmplitudeGlow;
extern juce::Colour placementModeGlow;
extern juce::Colour zoomPopupBorder;
extern juce::Colour zoomPopupTrackingLine;
extern juce::Colour zoomPopupPlaybackLine;
extern juce::Colour zoomPopupZeroLine;
extern juce::Colour volumeKnobFill;
extern juce::Colour volumeKnobTrack;
extern juce::Colour volumeKnobPointer;
extern juce::Colour quaternary;

struct ZoomHud {
    static juce::Colour background;
    static juce::Colour textActive;
    static juce::Colour textInactive;
};

struct VolumeFlame {
    static juce::Colour low;
    static juce::Colour mid;
    static juce::Colour high;
    static juce::Colour peak;
};

struct HintVox {
    static juce::Colour text;
};

struct Matrix {
    static juce::Colour ledActive;
    static juce::Colour ledInactive;
};
} // namespace Colors

struct Layout {
    struct Window {
        static int width;
        static int height;
    };
    static constexpr int windowBorderMargins = 15;
    static int buttonHeight;
    static int buttonWidth;
    static int clearButtonWidth;
    static float buttonCornerRadius;
    static constexpr float buttonOutlineThickness = 1.0f;
    static constexpr float connectorLineWidth = 2.0f;
    static constexpr float outlineThicknessMedium = 2.5f;
    static constexpr int cutTextWidth = 165;
    static constexpr int thresholdEditorWidth = 80;

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
        static float heightScale;
        static int pixelsPerSampleLow;
        static int pixelsPerSampleMedium;
        static int pixelsPerSampleHigh;
    };

    struct Glow {
        static constexpr float offsetFactor = 0.5f;
        static constexpr float mouseAlpha = 0.3f;
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
    };

    struct Matrix {
        static constexpr int squareSize = 6;
        static constexpr int rows = 4;
        static constexpr int startX = 4;
    };

    struct TopBar {
        static constexpr int volumeKnobSize = 80;
    };

    struct Fps {
        static constexpr int width = 125; // Widened to fit frame time
        static constexpr int height = 24;
        static constexpr int margin = 10;
        static constexpr float cornerRadius = 4.0f;
    };
};

namespace Animation {
constexpr float buttonHighlightedBrightness = 0.1f;
constexpr float buttonPressedDarkness = 0.1f;
constexpr float mouseAmplitudeLineLength = 50.0f;
constexpr float thresholdLineWidth = 100.0f;
} // namespace Animation

namespace Audio {
constexpr int thumbnailCacheSize = 5;
constexpr int thumbnailSizePixels = 512;
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

namespace Advanced {
extern bool showFpsOverlay;
extern int fpsOverlayX;
extern int fpsOverlayY;
extern juce::String fpsOverlayPosition;
} // namespace Advanced

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
} // namespace Labels

} // namespace Config

#endif
