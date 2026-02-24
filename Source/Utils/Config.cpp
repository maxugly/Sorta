

#include "Utils/Config.h"

namespace Config {

namespace Colors {
#if !defined(JUCE_HEADLESS)
const juce::Colour Window::background{juce::Colours::black};

const juce::Colour transparentBlack = juce::Colours::transparentBlack;
const juce::Colour solidBlack = juce::Colours::black;
const juce::Colour transparentWhite = juce::Colours::white.withAlpha(0.0f);

const juce::Colour Button::base{0xff5a5a5a};
const juce::Colour Button::on{juce::Colours::orange}; 
const juce::Colour Button::text{0xFFFFFFFF};
const juce::Colour Button::outline{0xff808080};
const juce::Colour Button::disabledBackground{0xff2a2a2a};
const juce::Colour Button::disabledText{0xff4a4a4a};
const juce::Colour Button::exit{juce::Colours::darkred};
const juce::Colour Button::clear{juce::Colours::red};
const juce::Colour Button::cutPlacement{0xffff1493};
const juce::Colour Button::cutActive{juce::Colours::darkorange}; 

const juce::Colour playbackText = juce::Colour(0xFF34FA11);
const juce::Colour textEditorBackground =
    juce::Colours::grey.withAlpha(Config::Layout::Text::backgroundAlpha);
const juce::Colour textEditorError = juce::Colours::red;
const juce::Colour textEditorWarning = juce::Colours::orange;
const juce::Colour textEditorOutOfRange = juce::Colours::orange;
const juce::Colour waveformPeak = juce::Colours::cyan;
const juce::Colour waveformCore = juce::Colour(0xff483d8b);
const juce::Colour playbackCursor = juce::Colours::lime;
const juce::Colour cutRegion = juce::Colours::darkorange.withAlpha(0.3f);
const juce::Colour cutLine = juce::Colours::orange;
const juce::Colour cutMarkerAuto = juce::Colours::orange.withAlpha(0.8f);
const juce::Colour cutMarkerHover = juce::Colours::orange;
const juce::Colour cutMarkerDrag = juce::Colours::green;
const juce::Colour mouseCursorLine = juce::Colours::yellow;
const juce::Colour mouseCursorHighlight = juce::Colours::darkorange.withAlpha(0.4f);
const juce::Colour mouseAmplitudeLine = juce::Colours::orange.brighter(0.5f);
const juce::Colour mousePlacementMode = juce::Colours::deeppink;
const juce::Colour thresholdLine = juce::Colour(0xffe600e6);
const juce::Colour thresholdRegion = juce::Colours::red.withAlpha(0.15f);
const juce::Colour statsBackground = juce::Colours::black.withAlpha(0.5f);
const juce::Colour statsText = juce::Colours::white;
const juce::Colour statsErrorText = juce::Colours::red;
const juce::Colour mouseAmplitudeGlow = juce::Colours::yellow;
const juce::Colour placementModeGlow = juce::Colours::red.withAlpha(0.7f);
const juce::Colour zoomPopupBorder = juce::Colour(0xff483d8b); // DarkSlateBlue
const juce::Colour zoomPopupTrackingLine = juce::Colours::orange;
const juce::Colour zoomPopupPlaybackLine = juce::Colours::lime;
const juce::Colour zoomPopupZeroLine = juce::Colours::grey.withAlpha(0.3f);

const juce::Colour Colors::ZoomHud::background = juce::Colours::black.withAlpha(0.6f);
const juce::Colour Colors::ZoomHud::textActive = juce::Colours::lime;
const juce::Colour Colors::ZoomHud::textInactive = juce::Colours::lightgrey;

const juce::Colour volumeKnobFill = juce::Colours::orange;
const juce::Colour volumeKnobTrack = juce::Colours::darkgrey;
const juce::Colour volumeKnobPointer = juce::Colours::white;

const juce::Colour Colors::VolumeFlame::low = juce::Colours::red;
const juce::Colour Colors::VolumeFlame::mid = juce::Colours::orange;
const juce::Colour Colors::VolumeFlame::high = juce::Colours::orange.brighter(0.3f);
const juce::Colour Colors::VolumeFlame::peak = juce::Colours::white;

const juce::Colour Colors::HintVox::text = juce::Colours::lightgrey;

const juce::Colour Colors::Matrix::ledActive = juce::Colours::lime;
const juce::Colour Colors::Matrix::ledInactive = juce::Colours::darkgrey;
#endif
} // namespace Colors

namespace Labels {
const juce::String appName = "audiofiler";
const juce::String appVersion = "0.0.001";
const juce::String defaultHint = "Ready.";
const juce::String openButton = "[D]ir";
const juce::String selectAudio = "Select Audio...";
const juce::String timeSeparator = " / ";
const juce::String openBracket = " (";
const juce::String closeBracket = ")";
const juce::String playButton = juce::CharPointer_UTF8("\xe2\x96\xb6");
const juce::String stopButton = juce::CharPointer_UTF8("\xe2\x8f\xb8");
const juce::String viewModeClassic = "[V]iew01";
const juce::String viewModeOverlay = "[V]iew02";
const juce::String channelViewMono = "[C]han 1";
const juce::String channelViewStereo = "[C]han 2";
const juce::String exitButton = "[E]xit";
const juce::String statsButton = "[S]tats";
const juce::String repeatButton = "[R]epeat";
const juce::String cutInButton = "[I]n";
const juce::String cutOutButton = "[O]ut";
const juce::String clearButton = "X";
const juce::String autoplayButton = "[A]utoPlay";
const juce::String autoCutInButton = "[AC In]";
const juce::String autoCutOutButton = "[AC Out]";
const juce::String cutButton = "[Cut]";
const juce::String lockLocked = juce::CharPointer_UTF8("\xef\x80\xa3");
const juce::String lockUnlocked = juce::CharPointer_UTF8("\xef\x81\x91");
const juce::String silenceThresholdInTooltip = "Silence Threshold In";
const juce::String silenceThresholdOutTooltip = "Silence Threshold Out";
const juce::String zoomPrefix = "Zoom: ";
const juce::String stepDefault = "Step: 1.0s (Default)";
const juce::String stepShift = "Step: 0.1s (Shift)";
const juce::String stepAlt = "Step: 10.0s (Alt)";
const juce::String stepCtrlShift = "Step: Sample (Ctrl+Shift)";
const juce::String statsFile = "File: ";
const juce::String statsSamples = "Samples Loaded: ";
const juce::String statsRate = "Sample Rate: ";
const juce::String statsHz = " Hz";
const juce::String statsChannels = "Channels: ";
const juce::String statsLength = "Length: ";
const juce::String statsPeak0 = "Approx Peak (Ch 0): ";
const juce::String statsPeak1 = "Approx Peak (Ch 1): ";
const juce::String statsMin = "Min: ";
const juce::String statsMax = ", Max: ";
const juce::String statsError = "No file loaded or error reading audio.";
const juce::String logNoAudio = "No audio loaded to detect silence.";
const juce::String logScanning = "SilenceDetector: Scanning ";
const juce::String logSamplesFor = " samples for ";
const juce::String logBoundary = " Silence Boundary.";
const juce::String logZeroLength = "SilenceDetector: Audio length is 0, cannot detect Silence Boundaries.";
const juce::String logStartSet = "Silence Boundary (Start) set to sample ";
const juce::String logEndSet = "Silence Boundary (End) set to sample ";
const juce::String logNoSound = "Could not detect any sound at ";
const juce::String logTooLarge = "SilenceDetector: Audio file is too large for automated Cut Point detection.";
const juce::String errorZeroLength = "Error: Audio file has zero length.";
const juce::String errorNoAudio = "No audio loaded.";
const juce::String scanningCutPoints = "Scanning for Cut Points...";
const juce::String noSilenceBoundaries = "No Silence Boundaries detected.";
} // namespace Labels

} // namespace Config
