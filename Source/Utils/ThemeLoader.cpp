#include "Utils/ThemeLoader.h"

void ThemeLoader::loadTheme(const juce::File& themeFile) {
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

        Config::Colors::Window::background = darker.darker(0.8f);
        Config::Colors::solidBlack = Config::Colors::Window::background.brighter(0.05f);
        Config::Colors::Button::base = primary;
        Config::Colors::Button::outline = lighter;
        Config::Colors::Button::disabledBackground = darker;
        Config::Colors::textEditorBackground = darker;
        Config::Colors::statsBackground = darker;
        Config::Colors::ZoomHud::background = darker;
        Config::Colors::volumeKnobTrack = darker;
        Config::Colors::Matrix::ledInactive = darker;
        Config::Colors::fpsBackground = darker;

        // Map the MS Paint inversion to Warnings/Exits
        Config::Colors::Button::clear = inverted;
        Config::Colors::textEditorError = inverted;
        Config::Colors::statsErrorText = inverted;
    }

    if (obj->hasProperty("secondaryColorHex")) {
        auto secondary = juce::Colour::fromString(obj->getProperty("secondaryColorHex").toString());

        Config::Colors::waveformPeak = secondary;
        if (obj->hasProperty("primaryColorHex")) {
            auto primary = juce::Colour::fromString(obj->getProperty("primaryColorHex").toString());
            // Fade from the secondary peak down into a slightly darkened primary core
            Config::Colors::waveformCore = primary.darker(0.2f);
        } else {
            Config::Colors::waveformCore = secondary.darker(0.8f);
        }
        Config::Colors::playbackCursor = secondary.brighter(0.4f);

        Config::Colors::Button::text = secondary;
        Config::Colors::Button::textActive = juce::Colours::white;
        Config::Colors::Button::on = secondary;
        Config::Colors::playbackText = secondary;
        Config::Colors::cutText = secondary;
        Config::Colors::thresholdText = secondary;
        Config::Colors::totalTimeText = secondary.darker(0.4f);
        Config::Colors::HintVox::text = secondary.brighter(0.2f);
        Config::Colors::Button::text = secondary;

        Config::Colors::cutLine = secondary;
        Config::Colors::cutMarkerAuto = secondary;
        Config::Colors::Matrix::ledActive = secondary;
        Config::Colors::volumeKnobFill = secondary;
        Config::Colors::Button::cutActive = secondary;

        Config::Colors::mouseCursorLine = secondary.brighter(0.5f);
        Config::Colors::mouseAmplitudeLine = secondary;
        Config::Colors::mouseAmplitudeGlow = secondary;
        Config::Colors::zoomPopupPlaybackLine = secondary;
        Config::Colors::thresholdLine = secondary;
    }

    if (obj->hasProperty("tertiaryColorHex")) {
        auto tertiary = juce::Colour::fromString(obj->getProperty("tertiaryColorHex").toString());
        Config::Colors::Button::text = tertiary;
    }

    if (obj->hasProperty("quaternaryColorHex")) {
        auto qColor = juce::Colour::fromString(obj->getProperty("quaternaryColorHex").toString());
        Config::Colors::quaternary = qColor;
        Config::Colors::cutText = qColor;
        Config::Colors::playbackText = qColor;
        Config::Colors::thresholdText = qColor;
        Config::Colors::totalTimeText = qColor.darker(0.4f);
        Config::Colors::HintVox::text = qColor.brighter(0.2f);
        Config::Colors::cutLine = qColor;
        Config::Colors::cutMarkerAuto = qColor;
        Config::Colors::thresholdLine = qColor;
    }

    setCol("windowBackgroundHex", Config::Colors::Window::background);
    setCol("waveformBackgroundHex", Config::Colors::solidBlack);
    setCol("waveformPeakHex", Config::Colors::waveformPeak);
    setCol("waveformCoreHex", Config::Colors::waveformCore);
    setCol("playbackCursorHex", Config::Colors::playbackCursor);
    setCol("cutRegionHex", Config::Colors::cutRegion);
    setCol("cutLineHex", Config::Colors::cutLine);
    setCol("cutMarkerAutoHex", Config::Colors::cutMarkerAuto);
    setCol("cutMarkerHoverHex", Config::Colors::cutMarkerHover);
    setCol("cutMarkerDragHex", Config::Colors::cutMarkerDrag);
    setCol("buttonBaseHex", Config::Colors::Button::base);
    setCol("buttonOnHex", Config::Colors::Button::on);
    setCol("buttonTextHex", Config::Colors::Button::text);
    setCol("buttonTextActiveHex", Config::Colors::Button::textActive);
    setCol("buttonOutlineHex", Config::Colors::Button::outline);
    setCol("buttonDisabledBackgroundHex", Config::Colors::Button::disabledBackground);
    setCol("buttonDisabledTextHex", Config::Colors::Button::disabledText);
    setCol("buttonClearHex", Config::Colors::Button::clear);
    setCol("buttonCutPlacementHex", Config::Colors::Button::cutPlacement);
    setCol("buttonCutActiveHex", Config::Colors::Button::cutActive);
    setCol("playbackTextHex", Config::Colors::playbackText);
    setCol("cutTextHex", Config::Colors::cutText);
    setCol("thresholdTextHex", Config::Colors::thresholdText);
    setCol("textEditorBackgroundHex", Config::Colors::textEditorBackground);
    setCol("textEditorErrorHex", Config::Colors::textEditorError);
    setCol("textEditorWarningHex", Config::Colors::textEditorWarning);
    setCol("textEditorOutOfRangeHex", Config::Colors::textEditorOutOfRange);
    setCol("fpsBackgroundHex", Config::Colors::fpsBackground);
    setCol("fpsTextHex", Config::Colors::fpsText);
    setCol("mouseCursorLineHex", Config::Colors::mouseCursorLine);
    setCol("mouseCursorHighlightHex", Config::Colors::mouseCursorHighlight);
    setCol("mouseAmplitudeLineHex", Config::Colors::mouseAmplitudeLine);
    setCol("mousePlacementModeHex", Config::Colors::mousePlacementMode);
    setCol("thresholdLineHex", Config::Colors::thresholdLine);
    setCol("thresholdRegionHex", Config::Colors::thresholdRegion);
    setCol("statsBackgroundHex", Config::Colors::statsBackground);
    setCol("statsTextHex", Config::Colors::statsText);
    setCol("statsErrorTextHex", Config::Colors::statsErrorText);
    setCol("mouseAmplitudeGlowHex", Config::Colors::mouseAmplitudeGlow);
    setCol("placementModeGlowHex", Config::Colors::placementModeGlow);
    setCol("zoomPopupBorderHex", Config::Colors::zoomPopupBorder);
    setCol("zoomPopupTrackingLineHex", Config::Colors::zoomPopupTrackingLine);
    setCol("zoomPopupPlaybackLineHex", Config::Colors::zoomPopupPlaybackLine);
    setCol("zoomPopupZeroLineHex", Config::Colors::zoomPopupZeroLine);
    setCol("zoomHudBackgroundHex", Config::Colors::ZoomHud::background);
    setCol("zoomHudTextActiveHex", Config::Colors::ZoomHud::textActive);
    setCol("zoomHudTextInactiveHex", Config::Colors::ZoomHud::textInactive);
    setCol("volumeKnobFillHex", Config::Colors::volumeKnobFill);
    setCol("volumeKnobTrackHex", Config::Colors::volumeKnobTrack);
    setCol("volumeKnobPointerHex", Config::Colors::volumeKnobPointer);
    setCol("volumeFlameLowHex", Config::Colors::VolumeFlame::low);
    setCol("volumeFlameMidHex", Config::Colors::VolumeFlame::mid);
    setCol("volumeFlameHighHex", Config::Colors::VolumeFlame::high);
    setCol("volumeFlamePeakHex", Config::Colors::VolumeFlame::peak);
    setCol("hintVoxTextHex", Config::Colors::HintVox::text);
    setCol("matrixLedActiveHex", Config::Colors::Matrix::ledActive);
    setCol("matrixLedInactiveHex", Config::Colors::Matrix::ledInactive);
    setCol("resizerBaseHex", Config::Colors::resizerBase);
    setCol("resizerLightHex", Config::Colors::resizerLight);
    setCol("resizerDarkHex", Config::Colors::resizerDark);
    // -----------------------------
#endif
}

void ThemeLoader::generateDefaultTheme(const juce::File& themeFile) {
    if (!themeFile.existsAsFile()) {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
#if !defined(JUCE_HEADLESS)
        obj->setProperty("windowBackgroundHex", Config::Colors::Window::background.toDisplayString(true));
        obj->setProperty("waveformPeakHex", Config::Colors::waveformPeak.toDisplayString(true));
        obj->setProperty("waveformCoreHex", Config::Colors::waveformCore.toDisplayString(true));
        obj->setProperty("playbackCursorHex", Config::Colors::playbackCursor.toDisplayString(true));
        obj->setProperty("cutRegionHex", Config::Colors::cutRegion.toDisplayString(true));
        obj->setProperty("cutLineHex", Config::Colors::cutLine.toDisplayString(true));
        obj->setProperty("cutMarkerAutoHex", Config::Colors::cutMarkerAuto.toDisplayString(true));
        obj->setProperty("cutMarkerHoverHex", Config::Colors::cutMarkerHover.toDisplayString(true));
        obj->setProperty("cutMarkerDragHex", Config::Colors::cutMarkerDrag.toDisplayString(true));
        obj->setProperty("buttonBaseHex", Config::Colors::Button::base.toDisplayString(true));
        obj->setProperty("buttonOnHex", Config::Colors::Button::on.toDisplayString(true));
        obj->setProperty("buttonTextHex", Config::Colors::Button::text.toDisplayString(true));
        obj->setProperty("buttonTextActiveHex", Config::Colors::Button::textActive.toDisplayString(true));
        obj->setProperty("buttonOutlineHex", Config::Colors::Button::outline.toDisplayString(true));
        obj->setProperty("buttonDisabledBackgroundHex", Config::Colors::Button::disabledBackground.toDisplayString(true));
        obj->setProperty("buttonDisabledTextHex", Config::Colors::Button::disabledText.toDisplayString(true));
        obj->setProperty("buttonExitHex", Config::Colors::Button::exit.toDisplayString(true));
        obj->setProperty("buttonClearHex", Config::Colors::Button::clear.toDisplayString(true));
        obj->setProperty("buttonCutPlacementHex", Config::Colors::Button::cutPlacement.toDisplayString(true));
        obj->setProperty("buttonCutActiveHex", Config::Colors::Button::cutActive.toDisplayString(true));
        obj->setProperty("playbackTextHex", Config::Colors::playbackText.toDisplayString(true));
        obj->setProperty("cutTextHex", Config::Colors::cutText.toDisplayString(true));
        obj->setProperty("thresholdTextHex", Config::Colors::thresholdText.toDisplayString(true));
        obj->setProperty("textEditorBackgroundHex", Config::Colors::textEditorBackground.toDisplayString(true));
        obj->setProperty("textEditorErrorHex", Config::Colors::textEditorError.toDisplayString(true));
        obj->setProperty("textEditorWarningHex", Config::Colors::textEditorWarning.toDisplayString(true));
        obj->setProperty("textEditorOutOfRangeHex", Config::Colors::textEditorOutOfRange.toDisplayString(true));
        obj->setProperty("fpsBackgroundHex", Config::Colors::fpsBackground.toDisplayString(true));
        obj->setProperty("fpsTextHex", Config::Colors::fpsText.toDisplayString(true));
        obj->setProperty("mouseCursorLineHex", Config::Colors::mouseCursorLine.toDisplayString(true));
        obj->setProperty("mouseCursorHighlightHex", Config::Colors::mouseCursorHighlight.toDisplayString(true));
        obj->setProperty("mouseAmplitudeLineHex", Config::Colors::mouseAmplitudeLine.toDisplayString(true));
        obj->setProperty("mousePlacementModeHex", Config::Colors::mousePlacementMode.toDisplayString(true));
        obj->setProperty("thresholdLineHex", Config::Colors::thresholdLine.toDisplayString(true));
        obj->setProperty("thresholdRegionHex", Config::Colors::thresholdRegion.toDisplayString(true));
        obj->setProperty("statsBackgroundHex", Config::Colors::statsBackground.toDisplayString(true));
        obj->setProperty("statsTextHex", Config::Colors::statsText.toDisplayString(true));
        obj->setProperty("statsErrorTextHex", Config::Colors::statsErrorText.toDisplayString(true));
        obj->setProperty("mouseAmplitudeGlowHex", Config::Colors::mouseAmplitudeGlow.toDisplayString(true));
        obj->setProperty("placementModeGlowHex", Config::Colors::placementModeGlow.toDisplayString(true));
        obj->setProperty("zoomPopupBorderHex", Config::Colors::zoomPopupBorder.toDisplayString(true));
        obj->setProperty("zoomPopupTrackingLineHex", Config::Colors::zoomPopupTrackingLine.toDisplayString(true));
        obj->setProperty("zoomPopupPlaybackLineHex", Config::Colors::zoomPopupPlaybackLine.toDisplayString(true));
        obj->setProperty("zoomPopupZeroLineHex", Config::Colors::zoomPopupZeroLine.toDisplayString(true));
        obj->setProperty("zoomHudBackgroundHex", Config::Colors::ZoomHud::background.toDisplayString(true));
        obj->setProperty("zoomHudTextActiveHex", Config::Colors::ZoomHud::textActive.toDisplayString(true));
        obj->setProperty("zoomHudTextInactiveHex", Config::Colors::ZoomHud::textInactive.toDisplayString(true));
        obj->setProperty("volumeKnobFillHex", Config::Colors::volumeKnobFill.toDisplayString(true));
        obj->setProperty("volumeKnobTrackHex", Config::Colors::volumeKnobTrack.toDisplayString(true));
        obj->setProperty("volumeKnobPointerHex", Config::Colors::volumeKnobPointer.toDisplayString(true));
        obj->setProperty("volumeFlameLowHex", Config::Colors::VolumeFlame::low.toDisplayString(true));
        obj->setProperty("volumeFlameMidHex", Config::Colors::VolumeFlame::mid.toDisplayString(true));
        obj->setProperty("volumeFlameHighHex", Config::Colors::VolumeFlame::high.toDisplayString(true));
        obj->setProperty("volumeFlamePeakHex", Config::Colors::VolumeFlame::peak.toDisplayString(true));
        obj->setProperty("hintVoxTextHex", Config::Colors::HintVox::text.toDisplayString(true));
        obj->setProperty("matrixLedActiveHex", Config::Colors::Matrix::ledActive.toDisplayString(true));
        obj->setProperty("matrixLedInactiveHex", Config::Colors::Matrix::ledInactive.toDisplayString(true));
        obj->setProperty("resizerBaseHex", Config::Colors::resizerBase.toDisplayString(true));
        obj->setProperty("resizerLightHex", Config::Colors::resizerLight.toDisplayString(true));
        obj->setProperty("resizerDarkHex", Config::Colors::resizerDark.toDisplayString(true));
#endif
        themeFile.replaceWithText(juce::JSON::toString(obj.get(), false));
    }
}
