

#include "Utils/Config.h"

namespace Config {

namespace Colors {
#if !defined(JUCE_HEADLESS)
juce::Colour Window::background{juce::Colours::black};

juce::Colour transparentBlack = juce::Colours::black;
juce::Colour solidBlack = juce::Colours::black;
juce::Colour transparentWhite = juce::Colours::black;

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
juce::Colour textEditorError = juce::Colours::red;
juce::Colour textEditorWarning = juce::Colours::orange;
juce::Colour textEditorOutOfRange = juce::Colours::orange;
juce::Colour waveformPeak = juce::Colours::red;
juce::Colour waveformCore = juce::Colour(0xff483d8b);
juce::Colour playbackCursor = juce::Colours::lime;
juce::Colour cutRegion = juce::Colours::darkorange;
juce::Colour cutLine = juce::Colours::orange;
juce::Colour cutMarkerAuto = juce::Colours::orange;
juce::Colour cutMarkerHover = juce::Colours::orange;
juce::Colour cutMarkerDrag = juce::Colours::green;
juce::Colour fpsBackground = juce::Colours::black;
juce::Colour fpsText = juce::Colours::lime;
juce::Colour mouseCursorLine = juce::Colours::white;
juce::Colour mouseCursorHighlight = juce::Colours::darkorange;
juce::Colour mouseAmplitudeLine = juce::Colours::orange.brighter(0.5f);
juce::Colour mousePlacementMode = juce::Colours::deeppink;
juce::Colour thresholdLine = juce::Colour(0xffe600e6);
juce::Colour thresholdRegion = juce::Colours::red;
juce::Colour statsBackground = juce::Colours::black;
juce::Colour statsText = juce::Colour(0xFF34FA11);
juce::Colour statsErrorText = juce::Colours::red;
juce::Colour mouseAmplitudeGlow = juce::Colours::yellow;
juce::Colour placementModeGlow = juce::Colours::red;
juce::Colour zoomPopupBorder = juce::Colour(0xff483d8b); // DarkSlateBlue
juce::Colour zoomPopupTrackingLine = juce::Colours::orange;
juce::Colour zoomPopupPlaybackLine = juce::Colours::lime;
juce::Colour zoomPopupZeroLine = juce::Colours::grey;

juce::Colour Colors::ZoomHud::background = juce::Colours::black;
juce::Colour Colors::ZoomHud::textActive = juce::Colours::lime;
juce::Colour Colors::ZoomHud::textInactive = juce::Colours::lightgrey;

juce::Colour volumeKnobFill = juce::Colours::orange;
juce::Colour volumeKnobTrack = juce::Colours::darkgrey;
juce::Colour volumeKnobPointer = juce::Colours::white;

juce::Colour Colors::VolumeFlame::low = juce::Colours::red;
juce::Colour Colors::VolumeFlame::mid = juce::Colours::orange;
juce::Colour Colors::VolumeFlame::high = juce::Colours::orange.brighter(0.3f);
juce::Colour Colors::VolumeFlame::peak = juce::Colours::white;

juce::Colour Colors::HintVox::text = juce::Colours::lightgrey;

juce::Colour Colors::Matrix::ledActive = juce::Colours::lime;
juce::Colour Colors::Matrix::ledInactive = juce::Colours::darkgrey;
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

int Layout::Window::width = 1920;
int Layout::Window::height = 1080;
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

void loadTheme(const juce::File& themeFile) {
    if (!themeFile.existsAsFile()) return;
    auto parsedJson = juce::JSON::parse(themeFile);
    if (!parsedJson.isObject()) return;
    auto* obj = parsedJson.getDynamicObject();

    auto setCol = [&](const char* p, juce::Colour& t) { 
        if (obj->hasProperty(p)) t = juce::Colour::fromString(obj->getProperty(p).toString()); 
    };

#if !defined(JUCE_HEADLESS)
    // --- MACRO THEME GENERATOR ---
    if (obj->hasProperty("primaryColorHex")) {
        auto primary = juce::Colour::fromString(obj->getProperty("primaryColorHex").toString());
        auto darker = primary.darker(0.6f);
        auto lighter = primary.brighter(0.3f);
        
        // MS Paint Invert (255 - R, 255 - G, 255 - B)
        auto inverted = juce::Colour((juce::uint8)(255 - primary.getRed()), 
                                     (juce::uint8)(255 - primary.getGreen()), 
                                     (juce::uint8)(255 - primary.getBlue()));

        Colors::Window::background = darker.darker(0.8f);
        Colors::solidBlack = Colors::Window::background.brighter(0.05f);
        Colors::Button::base = primary;
        Colors::Button::outline = lighter;
        Colors::Button::disabledBackground = darker;
        Colors::textEditorBackground = darker;
        Colors::statsBackground = darker;
        Colors::ZoomHud::background = darker;
        Colors::volumeKnobTrack = darker;
        Colors::Matrix::ledInactive = darker;
        Colors::fpsBackground = darker;

        // Map the MS Paint inversion to Warnings/Exits
        Colors::Button::exit = inverted;
        Colors::Button::clear = inverted;
        Colors::textEditorError = inverted;
        Colors::statsErrorText = inverted;
    }

    if (obj->hasProperty("secondaryColorHex")) {
        auto secondary = juce::Colour::fromString(obj->getProperty("secondaryColorHex").toString());

        Colors::waveformPeak = secondary;
        if (obj->hasProperty("primaryColorHex")) {
            auto primary = juce::Colour::fromString(obj->getProperty("primaryColorHex").toString());
            // Fade from the secondary peak down into a slightly darkened primary core
            Colors::waveformCore = primary.darker(0.2f); 
        } else {
            Colors::waveformCore = secondary.darker(0.8f);
        }
        Colors::playbackCursor = secondary.brighter(0.4f);
        
        Colors::Button::text = secondary;
        Colors::Button::on = secondary;
        Colors::playbackText = secondary;
        Colors::cutLine = secondary;
        Colors::cutMarkerAuto = secondary;
        Colors::Matrix::ledActive = secondary;
        Colors::volumeKnobFill = secondary;
        
        Colors::mouseCursorLine = secondary.brighter(0.5f);
        Colors::mouseAmplitudeLine = secondary;
        Colors::mouseAmplitudeGlow = secondary;
        Colors::zoomPopupPlaybackLine = secondary;
    }

    if (obj->hasProperty("tertiaryColorHex")) {
        auto tertiary = juce::Colour::fromString(obj->getProperty("tertiaryColorHex").toString());
        Colors::Button::text = tertiary;
    }
    // -----------------------------

    setCol("windowBackgroundHex", Colors::Window::background);
    setCol("waveformBackgroundHex", Colors::solidBlack);
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
    setCol("textEditorErrorHex", Colors::textEditorError);
    setCol("textEditorWarningHex", Colors::textEditorWarning);
    setCol("textEditorOutOfRangeHex", Colors::textEditorOutOfRange);
    setCol("fpsBackgroundHex", Colors::fpsBackground);
    setCol("fpsTextHex", Colors::fpsText);
    setCol("mouseCursorLineHex", Colors::mouseCursorLine);
    setCol("mouseCursorHighlightHex", Colors::mouseCursorHighlight);
    setCol("mouseAmplitudeLineHex", Colors::mouseAmplitudeLine);
    setCol("mousePlacementModeHex", Colors::mousePlacementMode);
    setCol("thresholdLineHex", Colors::thresholdLine);
    setCol("thresholdRegionHex", Colors::thresholdRegion);
    setCol("statsBackgroundHex", Colors::statsBackground);
    setCol("statsTextHex", Colors::statsText);
    setCol("statsErrorTextHex", Colors::statsErrorText);
    setCol("mouseAmplitudeGlowHex", Colors::mouseAmplitudeGlow);
    setCol("placementModeGlowHex", Colors::placementModeGlow);
    setCol("zoomPopupBorderHex", Colors::zoomPopupBorder);
    setCol("zoomPopupTrackingLineHex", Colors::zoomPopupTrackingLine);
    setCol("zoomPopupPlaybackLineHex", Colors::zoomPopupPlaybackLine);
    setCol("zoomPopupZeroLineHex", Colors::zoomPopupZeroLine);
    setCol("zoomHudBackgroundHex", Colors::ZoomHud::background);
    setCol("zoomHudTextActiveHex", Colors::ZoomHud::textActive);
    setCol("zoomHudTextInactiveHex", Colors::ZoomHud::textInactive);
    setCol("volumeKnobFillHex", Colors::volumeKnobFill);
    setCol("volumeKnobTrackHex", Colors::volumeKnobTrack);
    setCol("volumeKnobPointerHex", Colors::volumeKnobPointer);
    setCol("volumeFlameLowHex", Colors::VolumeFlame::low);
    setCol("volumeFlameMidHex", Colors::VolumeFlame::mid);
    setCol("volumeFlameHighHex", Colors::VolumeFlame::high);
    setCol("volumeFlamePeakHex", Colors::VolumeFlame::peak);
    setCol("hintVoxTextHex", Colors::HintVox::text);
    setCol("matrixLedActiveHex", Colors::Matrix::ledActive);
    setCol("matrixLedInactiveHex", Colors::Matrix::ledInactive);
#endif
}

void initializeConfigs() {
    auto configDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(".config/audiofiler");
    if (!configDir.exists()) configDir.createDirectory();

    auto themesDir = configDir.getChildFile("themes");
    if (!themesDir.exists()) themesDir.createDirectory();

    auto settingsFile = configDir.getChildFile("settings.conf");
    auto themeFile = themesDir.getChildFile("default.conf");
    auto languageFile = configDir.getChildFile("language.conf");
    auto advancedFile = configDir.getChildFile("advanced.conf");

    // --- Helper: Parsing Logic ---
    auto parseFile = [&](const juce::File& file) {
        if (!file.existsAsFile()) return;
        auto parsedJson = juce::JSON::parse(file);
        if (!parsedJson.isObject()) return;
        auto* obj = parsedJson.getDynamicObject();
        
        auto setInt = [&](const char* p, int& t) { if (obj->hasProperty(p)) t = obj->getProperty(p); };
        auto setFloat = [&](const char* p, float& t) { if (obj->hasProperty(p)) t = obj->getProperty(p); };
        auto setStr = [&](const char* p, juce::String& t) { if (obj->hasProperty(p)) t = obj->getProperty(p).toString(); };
        auto setBool = [&](const char* p, bool& t) { if (obj->hasProperty(p)) t = obj->getProperty(p); };

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

        setBool("showFpsOverlay", Advanced::showFpsOverlay);
        setInt("fpsOverlayX", Advanced::fpsOverlayX);
        setInt("fpsOverlayY", Advanced::fpsOverlayY);
        setStr("fpsOverlayPosition", Advanced::fpsOverlayPosition);
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
        obj->setProperty("playbackCursorHex", Colors::playbackCursor.toDisplayString(true));
        obj->setProperty("cutRegionHex", Colors::cutRegion.toDisplayString(true));
        obj->setProperty("cutLineHex", Colors::cutLine.toDisplayString(true));
        obj->setProperty("cutMarkerAutoHex", Colors::cutMarkerAuto.toDisplayString(true));
        obj->setProperty("cutMarkerHoverHex", Colors::cutMarkerHover.toDisplayString(true));
        obj->setProperty("cutMarkerDragHex", Colors::cutMarkerDrag.toDisplayString(true));
        obj->setProperty("buttonBaseHex", Colors::Button::base.toDisplayString(true));
        obj->setProperty("buttonOnHex", Colors::Button::on.toDisplayString(true));
        obj->setProperty("buttonTextHex", Colors::Button::text.toDisplayString(true));
        obj->setProperty("buttonOutlineHex", Colors::Button::outline.toDisplayString(true));
        obj->setProperty("buttonDisabledBackgroundHex", Colors::Button::disabledBackground.toDisplayString(true));
        obj->setProperty("buttonDisabledTextHex", Colors::Button::disabledText.toDisplayString(true));
        obj->setProperty("buttonExitHex", Colors::Button::exit.toDisplayString(true));
        obj->setProperty("buttonClearHex", Colors::Button::clear.toDisplayString(true));
        obj->setProperty("buttonCutPlacementHex", Colors::Button::cutPlacement.toDisplayString(true));
        obj->setProperty("buttonCutActiveHex", Colors::Button::cutActive.toDisplayString(true));
        obj->setProperty("playbackTextHex", Colors::playbackText.toDisplayString(true));
        obj->setProperty("textEditorBackgroundHex", Colors::textEditorBackground.toDisplayString(true));
        obj->setProperty("textEditorErrorHex", Colors::textEditorError.toDisplayString(true));
        obj->setProperty("textEditorWarningHex", Colors::textEditorWarning.toDisplayString(true));
        obj->setProperty("textEditorOutOfRangeHex", Colors::textEditorOutOfRange.toDisplayString(true));
        obj->setProperty("fpsBackgroundHex", Colors::fpsBackground.toDisplayString(true));
        obj->setProperty("fpsTextHex", Colors::fpsText.toDisplayString(true));
        obj->setProperty("mouseCursorLineHex", Colors::mouseCursorLine.toDisplayString(true));
        obj->setProperty("mouseCursorHighlightHex", Colors::mouseCursorHighlight.toDisplayString(true));
        obj->setProperty("mouseAmplitudeLineHex", Colors::mouseAmplitudeLine.toDisplayString(true));
        obj->setProperty("mousePlacementModeHex", Colors::mousePlacementMode.toDisplayString(true));
        obj->setProperty("thresholdLineHex", Colors::thresholdLine.toDisplayString(true));
        obj->setProperty("thresholdRegionHex", Colors::thresholdRegion.toDisplayString(true));
        obj->setProperty("statsBackgroundHex", Colors::statsBackground.toDisplayString(true));
        obj->setProperty("statsTextHex", Colors::statsText.toDisplayString(true));
        obj->setProperty("statsErrorTextHex", Colors::statsErrorText.toDisplayString(true));
        obj->setProperty("mouseAmplitudeGlowHex", Colors::mouseAmplitudeGlow.toDisplayString(true));
        obj->setProperty("placementModeGlowHex", Colors::placementModeGlow.toDisplayString(true));
        obj->setProperty("zoomPopupBorderHex", Colors::zoomPopupBorder.toDisplayString(true));
        obj->setProperty("zoomPopupTrackingLineHex", Colors::zoomPopupTrackingLine.toDisplayString(true));
        obj->setProperty("zoomPopupPlaybackLineHex", Colors::zoomPopupPlaybackLine.toDisplayString(true));
        obj->setProperty("zoomPopupZeroLineHex", Colors::zoomPopupZeroLine.toDisplayString(true));
        obj->setProperty("zoomHudBackgroundHex", Colors::ZoomHud::background.toDisplayString(true));
        obj->setProperty("zoomHudTextActiveHex", Colors::ZoomHud::textActive.toDisplayString(true));
        obj->setProperty("zoomHudTextInactiveHex", Colors::ZoomHud::textInactive.toDisplayString(true));
        obj->setProperty("volumeKnobFillHex", Colors::volumeKnobFill.toDisplayString(true));
        obj->setProperty("volumeKnobTrackHex", Colors::volumeKnobTrack.toDisplayString(true));
        obj->setProperty("volumeKnobPointerHex", Colors::volumeKnobPointer.toDisplayString(true));
        obj->setProperty("volumeFlameLowHex", Colors::VolumeFlame::low.toDisplayString(true));
        obj->setProperty("volumeFlameMidHex", Colors::VolumeFlame::mid.toDisplayString(true));
        obj->setProperty("volumeFlameHighHex", Colors::VolumeFlame::high.toDisplayString(true));
        obj->setProperty("volumeFlamePeakHex", Colors::VolumeFlame::peak.toDisplayString(true));
        obj->setProperty("hintVoxTextHex", Colors::HintVox::text.toDisplayString(true));
        obj->setProperty("matrixLedActiveHex", Colors::Matrix::ledActive.toDisplayString(true));
        obj->setProperty("matrixLedInactiveHex", Colors::Matrix::ledInactive.toDisplayString(true));
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

    if (!advancedFile.existsAsFile()) {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty("showFpsOverlay", Advanced::showFpsOverlay);
        obj->setProperty("fpsOverlayX", Advanced::fpsOverlayX);
        obj->setProperty("fpsOverlayY", Advanced::fpsOverlayY);
        obj->setProperty("fpsOverlayPosition", Advanced::fpsOverlayPosition);
        advancedFile.replaceWithText(juce::JSON::toString(obj.get(), false));
    }

    parseFile(settingsFile);
    loadTheme(themeFile);
    parseFile(languageFile);
    parseFile(advancedFile);
}

bool Advanced::showFpsOverlay = true;
int Advanced::fpsOverlayX = 10;
int Advanced::fpsOverlayY = 10;
juce::String Advanced::fpsOverlayPosition = "topC";

} // namespace Config
