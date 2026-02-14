#include "Config.h"

namespace Config {

    //==============================================================================
    // Colors
    //==============================================================================

    // Window
    const juce::Colour Colors::Window::background = juce::Colours::black;

    // Playback
    const juce::Colour Colors::Playback::text = juce::Colour(0xFF34FA11); // Bright green
    const juce::Colour Colors::Playback::cursor = juce::Colours::lime;
    const juce::Colour Colors::Playback::cursorGlowStart = juce::Colours::lime.withAlpha(0.0f);
    const juce::Colour Colors::Playback::cursorGlowEnd = juce::Colours::lime.withAlpha(0.5f);

    // Button
    const juce::Colour Colors::Button::base = juce::Colour(0xff5a5a5a);
    const juce::Colour Colors::Button::on = juce::Colour(0xff00bfff); // Deep Sky Blue
    const juce::Colour Colors::Button::text = juce::Colour(0xFFFFFFFF); // White
    const juce::Colour Colors::Button::outline = juce::Colour(0xff808080); // Medium Grey
    const juce::Colour Colors::Button::disabledBackground = juce::Colour(0xff2a2a2a);
    const juce::Colour Colors::Button::disabledText = juce::Colour(0xff4a4a4a);
    const juce::Colour Colors::Button::exit = juce::Colours::darkred;
    const juce::Colour Colors::Button::clear = juce::Colours::red;
    const juce::Colour Colors::Button::loopPlacement = juce::Colour(0xffff1493); // Deep Pink
    const juce::Colour Colors::Button::loopActive = juce::Colour(0xff0066cc); // Moderate Blue

    // TextEditor
    const juce::Colour Colors::TextEditor::background = juce::Colours::grey.withAlpha(Config::Layout::Text::backgroundAlpha);
    const juce::Colour Colors::TextEditor::error = juce::Colours::red;
    const juce::Colour Colors::TextEditor::warning = juce::Colours::orange;
    const juce::Colour Colors::TextEditor::outOfRange = juce::Colours::orange;

    // Waveform
    const juce::Colour Colors::Waveform::wave = juce::Colours::deeppink;

    // Loop
    const juce::Colour Colors::Loop::region = juce::Colour(0xff0066cc).withAlpha(0.3f);
    const juce::Colour Colors::Loop::line = juce::Colours::blue;
    const juce::Colour Colors::Loop::markerAuto = juce::Colour(0xff00bfff);
    const juce::Colour Colors::Loop::markerHover = juce::Colours::teal;
    const juce::Colour Colors::Loop::markerDrag = juce::Colours::green;

    // Mouse
    const juce::Colour Colors::Mouse::cursorLine = juce::Colours::yellow;
    const juce::Colour Colors::Mouse::cursorHighlight = juce::Colours::darkorange.withAlpha(0.4f);
    const juce::Colour Colors::Mouse::amplitudeLine = juce::Colours::orange.brighter(0.5f);
    const juce::Colour Colors::Mouse::placementMode = juce::Colours::deeppink;
    const juce::Colour Colors::Mouse::amplitudeGlow = juce::Colours::yellow;
    const juce::Colour Colors::Mouse::placementModeGlow = juce::Colours::red.withAlpha(0.7f);

    // Silence
    const juce::Colour Colors::Silence::line = juce::Colour(0xffe600e6);
    const juce::Colour Colors::Silence::region = juce::Colours::red.withAlpha(0.15f);

    // Stats
    const juce::Colour Colors::Stats::background = juce::Colours::black.withAlpha(0.5f);
    const juce::Colour Colors::Stats::text = juce::Colours::white;
    const juce::Colour Colors::Stats::errorText = juce::Colours::red;

    // Zoom
    const juce::Colour Colors::Zoom::border = juce::Colours::blue;
    const juce::Colour Colors::Zoom::indicator = juce::Colours::white;
    const juce::Colour Colors::Zoom::shadowOuter = juce::Colours::black.withAlpha(0.5f);
    const juce::Colour Colors::Zoom::shadowInner = juce::Colours::black;
    const juce::Colour Colors::Zoom::trackingLine = juce::Colours::dodgerblue;
    const juce::Colour Colors::Zoom::playbackLine = juce::Colours::lime;
    const juce::Colour Colors::Zoom::zeroLine = juce::Colours::grey.withAlpha(0.3f);

    // HazyBox
    const juce::Colour Colors::HazyBox::fade = juce::Colours::black.withAlpha(0.0f);
    const juce::Colour Colors::HazyBox::line = juce::Colours::cyan;

    //==============================================================================
    // Labels
    //==============================================================================
    namespace Labels {
        const juce::String openButton = "[D]ir";
        const juce::String playButton = juce::CharPointer_UTF8 ("\xe2\x96\xb6");
        const juce::String stopButton = juce::CharPointer_UTF8 ("\xe2\x8f\xb8");
        const juce::String viewModeClassic = "[V]iew01";
        const juce::String viewModeOverlay = "[V]iew02";
        const juce::String channelViewMono = "[C]han 1";
        const juce::String channelViewStereo = "[C]han 2";
        const juce::String qualityButton = "[Q]ual";
        const juce::String qualityHigh = "[Q]ual H";
        const juce::String qualityMedium = "[Q]ual M";
        const juce::String qualityLow = "[Q]ual L";
        const juce::String exitButton = "[E]xit";
        const juce::String statsButton = "[S]tats";
        const juce::String loopButton = "[L]oop";
        const juce::String loopInButton = "[I]n";
        const juce::String loopOutButton = "[O]ut";
        const juce::String clearButton = "X";
        const juce::String detectInButton = "Detect In";
        const juce::String detectOutButton = "Detect Out";
        const juce::String autoplayButton = "[A]utoPlay";
        const juce::String autoCutInButton = "[AC In]";
        const juce::String autoCutOutButton = "[AC Out]";
        const juce::String cutButton = "[Cut]";
    }

}
