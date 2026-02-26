

#include "Utils/Config.h"

namespace Config {

namespace Colors {
#if !defined(JUCE_HEADLESS)
juce::Colour Window::background{juce::Colours::black};

const juce::Colour transparentBlack = juce::Colours::black;
const juce::Colour solidBlack = juce::Colours::black;
const juce::Colour transparentWhite = juce::Colours::black;

juce::Colour Button::base{0xff5a5a5a};
juce::Colour Button::on{juce::Colours::orange}; 
juce::Colour Button::text{0xFF34FA11};
juce::Colour Button::outline{0xff808080};
juce::Colour Button::disabledBackground{0xff2a2a2a};
juce::Colour Button::disabledText{0xff4a4a4a};
juce::Colour Button::exit{juce::Colours::darkred};
juce::Colour Button::clear{juce::Colours::red};
juce::Colour Button::cutPlacement{0xffff1493};
juce::Colour Button::cutActive{juce::Colours::darkorange}; 

juce::Colour playbackText = juce::Colour(0xFF34FA11);
juce::Colour textEditorBackground = juce::Colour(0xff333333);
const juce::Colour textEditorError = juce::Colours::red;
const juce::Colour textEditorWarning = juce::Colours::orange;
const juce::Colour textEditorOutOfRange = juce::Colours::orange;
juce::Colour waveformPeak = juce::Colours::cyan;
juce::Colour waveformCore = juce::Colour(0xff483d8b);
juce::Colour playbackCursor = juce::Colours::lime;
juce::Colour cutRegion = juce::Colours::darkorange;
juce::Colour cutLine = juce::Colours::orange;
juce::Colour cutMarkerAuto = juce::Colours::orange;
juce::Colour cutMarkerHover = juce::Colours::orange;
juce::Colour cutMarkerDrag = juce::Colours::green;
const juce::Colour fpsBackground = juce::Colours::black;
const juce::Colour fpsText = juce::Colours::lime;
const juce::Colour mouseCursorLine = juce::Colours::white;
const juce::Colour mouseCursorHighlight = juce::Colours::darkorange;
const juce::Colour mouseAmplitudeLine = juce::Colours::orange.brighter(0.5f);
const juce::Colour mousePlacementMode = juce::Colours::deeppink;
const juce::Colour thresholdLine = juce::Colour(0xffe600e6);
const juce::Colour thresholdRegion = juce::Colours::red;
const juce::Colour statsBackground = juce::Colours::black;
const juce::Colour statsText = juce::Colour(0xFF34FA11);
const juce::Colour statsErrorText = juce::Colours::red;
const juce::Colour mouseAmplitudeGlow = juce::Colours::yellow;
const juce::Colour placementModeGlow = juce::Colours::red;
const juce::Colour zoomPopupBorder = juce::Colour(0xff483d8b); // DarkSlateBlue
const juce::Colour zoomPopupTrackingLine = juce::Colours::orange;
const juce::Colour zoomPopupPlaybackLine = juce::Colours::lime;
const juce::Colour zoomPopupZeroLine = juce::Colours::grey;

const juce::Colour Colors::ZoomHud::background = juce::Colours::black;
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
juce::String appName = "audiofiler";
juce::String appVersion = "0.0.001";
juce::String defaultHint = "Ready.";
juce::String openButton = "[D]ir";
juce::String selectAudio = "Select Audio...";
juce::String timeSeparator = " / ";
juce::String openBracket = " (";
juce::String closeBracket = ")";
juce::String playButton = juce::CharPointer_UTF8("\xe2\x96\xb6");
juce::String stopButton = juce::CharPointer_UTF8("\xe2\x8f\xb8");
juce::String viewModeClassic = "[V]iew01";
juce::String viewModeOverlay = "[V]iew02";
juce::String channelViewMono = "[C]han 1";
juce::String channelViewStereo = "[C]han 2";
juce::String exitButton = "[E]xit";
juce::String statsButton = "[S]tats";
juce::String repeatButton = "[R]epeat";
juce::String cutInButton = "[I]n";
juce::String cutOutButton = "[O]ut";
juce::String clearButton = "X";
juce::String autoplayButton = "[A]utoPlay";
juce::String autoCutInButton = "[AC In]";
juce::String autoCutOutButton = "[AC Out]";
juce::String cutButton = "[Cut]";
juce::String lockLocked = juce::CharPointer_UTF8("\xef\x80\xa3");
juce::String lockUnlocked = juce::CharPointer_UTF8("\xef\x81\x91");
juce::String silenceThresholdInTooltip = "Silence Threshold In";
juce::String silenceThresholdOutTooltip = "Silence Threshold Out";
juce::String zoomPrefix = "Zoom: ";
juce::String stepDefault = "Step: 1.0s (Default)";
juce::String stepShift = "Step: 0.1s (Shift)";
juce::String stepAlt = "Step: 10.0s (Alt)";
juce::String stepCtrlShift = "Step: Sample (Ctrl+Shift)";
juce::String statsFile = "File: ";
juce::String statsSamples = "Samples Loaded: ";
juce::String statsRate = "Sample Rate: ";
juce::String statsHz = " Hz";
juce::String statsChannels = "Channels: ";
juce::String statsLength = "Length: ";
juce::String statsPeak0 = "Approx Peak (Ch 0): ";
juce::String statsPeak1 = "Approx Peak (Ch 1): ";
juce::String statsMin = "Min: ";
juce::String statsMax = ", Max: ";
juce::String statsError = "No file loaded or error reading audio.";
juce::String logNoAudio = "No audio loaded to detect silence.";
juce::String logScanning = "SilenceDetector: Scanning ";
juce::String logSamplesFor = " samples for ";
juce::String logBoundary = " Silence Boundary.";
juce::String logZeroLength = "SilenceDetector: Audio length is 0, cannot detect Silence Boundaries.";
juce::String logStartSet = "Silence Boundary (Start) set to sample ";
juce::String logEndSet = "Silence Boundary (End) set to sample ";
juce::String logNoSound = "Could not detect any sound at ";
juce::String logTooLarge = "SilenceDetector: Audio file is too large for automated Cut Point detection.";
juce::String errorZeroLength = "Error: Audio file has zero length.";
juce::String errorNoAudio = "No audio loaded.";
juce::String scanningCutPoints = "Scanning for Cut Points...";
juce::String noSilenceBoundaries = "No Silence Boundaries detected.";
} // namespace Labels

int Layout::Window::width = 1080;
int Layout::Window::height = 800;
float Layout::Waveform::heightScale = 0.5f;
int Layout::Waveform::pixelsPerSampleLow = 4;
int Layout::Waveform::pixelsPerSampleMedium = 2;
int Layout::Waveform::pixelsPerSampleHigh = 1;
float Layout::Glow::thickness = 4.0f;
float Layout::Glow::cutLineGlowThickness = 4.0f;
float Layout::Glow::cutMarkerWidthThin = 1.0f;
int Layout::buttonHeight = 30;
int Layout::buttonWidth = 80;
int Layout::clearButtonWidth = 25;
float Layout::buttonCornerRadius = 5.0f;

void initializeConfigs() {
    auto configDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(".config/audiofiler");
    if (!configDir.exists()) configDir.createDirectory();

    auto settingsFile = configDir.getChildFile("settings.conf");
    auto themeFile = configDir.getChildFile("theme.conf");
    auto languageFile = configDir.getChildFile("language.conf");

    // --- Helper: Parsing Logic ---
    auto parseFile = [&](const juce::File& file) {
        if (!file.existsAsFile()) return;
        auto parsedJson = juce::JSON::parse(file);
        if (!parsedJson.isObject()) return;
        auto* obj = parsedJson.getDynamicObject();
        
        auto setInt = [&](const char* p, int& t) { if (obj->hasProperty(p)) t = obj->getProperty(p); };
        auto setFloat = [&](const char* p, float& t) { if (obj->hasProperty(p)) t = obj->getProperty(p); };
        auto setStr = [&](const char* p, juce::String& t) { if (obj->hasProperty(p)) t = obj->getProperty(p).toString(); };
        auto setCol = [&](const char* p, juce::Colour& t) { 
            if (obj->hasProperty(p)) t = juce::Colour::fromString(obj->getProperty(p).toString()); 
        };

        setInt("windowWidth", Layout::Window::width);
        setInt("windowHeight", Layout::Window::height);
        setFloat("waveformHeightScale", Layout::Waveform::heightScale);
        setInt("waveformPixelsPerSampleLow", Layout::Waveform::pixelsPerSampleLow);
        setInt("waveformPixelsPerSampleMedium", Layout::Waveform::pixelsPerSampleMedium);
        setInt("waveformPixelsPerSampleHigh", Layout::Waveform::pixelsPerSampleHigh);
        setFloat("glowThickness", Layout::Glow::thickness);
        setFloat("cutLineGlowThickness", Layout::Glow::cutLineGlowThickness);
        setFloat("cutMarkerWidthThin", Layout::Glow::cutMarkerWidthThin);
        setInt("buttonHeight", Layout::buttonHeight);
        setInt("buttonWidth", Layout::buttonWidth);
        setFloat("buttonCornerRadius", Layout::buttonCornerRadius);

        setStr("labelOpenButton", Labels::openButton);
        setStr("labelExitButton", Labels::exitButton);
        setStr("labelStatsButton", Labels::statsButton);
        setStr("labelRepeatButton", Labels::repeatButton);
        setStr("labelCutInButton", Labels::cutInButton);
        setStr("labelCutOutButton", Labels::cutOutButton);
        setStr("labelClearButton", Labels::clearButton);
        setStr("labelAutoplayButton", Labels::autoplayButton);
        setStr("labelAutoCutInButton", Labels::autoCutInButton);
        setStr("labelAutoCutOutButton", Labels::autoCutOutButton);
        setStr("labelCutButton", Labels::cutButton);

#if !defined(JUCE_HEADLESS)
        setCol("windowBackgroundHex", Colors::Window::background);
        setCol("waveformPeakHex", Colors::waveformPeak);
        setCol("waveformCoreHex", Colors::waveformCore);
        setCol("playbackCursorHex", Colors::playbackCursor);
        setCol("cutRegionHex", Colors::cutRegion);
        setCol("cutLineHex", Colors::cutLine);
        setCol("cutMarkerAutoHex", Colors::cutMarkerAuto);
        setCol("cutMarkerHoverHex", Colors::cutMarkerHover);
        setCol("cutMarkerDragHex", Colors::cutMarkerDrag);
        setCol("buttonBaseHex", Colors::Button::base);
        setCol("buttonOnHex", Colors::Button::on);
        setCol("buttonTextHex", Colors::Button::text);
        setCol("buttonOutlineHex", Colors::Button::outline);
        setCol("buttonDisabledBackgroundHex", Colors::Button::disabledBackground);
        setCol("buttonDisabledTextHex", Colors::Button::disabledText);
        setCol("buttonExitHex", Colors::Button::exit);
        setCol("buttonClearHex", Colors::Button::clear);
        setCol("buttonCutPlacementHex", Colors::Button::cutPlacement);
        setCol("buttonCutActiveHex", Colors::Button::cutActive);
        setCol("playbackTextHex", Colors::playbackText);
        setCol("textEditorBackgroundHex", Colors::textEditorBackground);
#endif
    };

    // --- Helper: Auto-Generation ---
    if (!settingsFile.existsAsFile()) {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty("windowWidth", Layout::Window::width);
        obj->setProperty("windowHeight", Layout::Window::height);
        obj->setProperty("buttonHeight", Layout::buttonHeight);
        obj->setProperty("buttonWidth", Layout::buttonWidth);
        settingsFile.replaceWithText(juce::JSON::toString(obj.get(), false));
    }

    if (!themeFile.existsAsFile()) {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
#if !defined(JUCE_HEADLESS)
        obj->setProperty("windowBackgroundHex", Colors::Window::background.toDisplayString(true));
        obj->setProperty("waveformPeakHex", Colors::waveformPeak.toDisplayString(true));
        obj->setProperty("waveformCoreHex", Colors::waveformCore.toDisplayString(true));
        obj->setProperty("buttonBaseHex", Colors::Button::base.toDisplayString(true));
#endif
        themeFile.replaceWithText(juce::JSON::toString(obj.get(), false));
    }

    if (!languageFile.existsAsFile()) {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty("labelOpenButton", Labels::openButton);
        obj->setProperty("labelExitButton", Labels::exitButton);
        obj->setProperty("labelCutButton", Labels::cutButton);
        languageFile.replaceWithText(juce::JSON::toString(obj.get(), false));
    }

    parseFile(settingsFile);
    parseFile(themeFile);
    parseFile(languageFile);
}

} // namespace Config
