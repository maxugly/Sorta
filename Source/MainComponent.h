#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "DrunkDrawnLookAndFeel.h"

class MainComponent  : public juce::AudioAppComponent,
                       public juce::ChangeListener,
                       public juce::Timer
{
public:
    enum class ViewMode { Classic, Overlay };

    MainComponent() : thumbnailCache (5),
                      thumbnail (512, formatManager, thumbnailCache)    {
        // 1. Formats
        formatManager.registerFormat (new juce::WavAudioFormat(), false);
        formatManager.registerFormat (new juce::AiffAudioFormat(), false);
        formatManager.registerFormat (new juce::FlacAudioFormat(), false);
        formatManager.registerFormat (new juce::OggVorbisAudioFormat(), false);
        formatManager.registerFormat (new juce::MP3AudioFormat(), false);

        thumbnail.addChangeListener (this);

        setLookAndFeel(&drunkLF);

        // 2. Buttons
        addAndMakeVisible (openButton);
        openButton.setButtonText ("Open");
        openButton.onClick = [this] { openButtonClicked(); };

        addAndMakeVisible (playStopButton);
        updateButtonText();
        playStopButton.onClick = [this] { playStopButtonClicked(); };
        playStopButton.setEnabled (false);

        addAndMakeVisible (modeButton);
        modeButton.setButtonText ("View");
        modeButton.onClick = [this] {
            currentMode = (currentMode == ViewMode::Classic) ? ViewMode::Overlay : ViewMode::Classic;
            resized();
            repaint();
        };

        // NEW: Exit Button
        addAndMakeVisible (exitButton);
        exitButton.setButtonText ("Exit");
        exitButton.setColour (juce::TextButton::buttonColourId, juce::Colours::darkred);
        exitButton.onClick = [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };

        addAndMakeVisible(statsButton);
        statsButton.setButtonText("Stats");
        statsButton.setClickingTogglesState(true);
        statsButton.onClick = [this] {
            showStats = statsButton.getToggleState();
            resized();
        };
        // Debug Display
        addAndMakeVisible(statsDisplay);
        statsDisplay.setReadOnly(true);
        statsDisplay.setMultiLine(true);
        statsDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
        statsDisplay.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        statsDisplay.setVisible(false); // Hidden by default

        openButton.setName("openBtn");
        playStopButton.setName("playBtn");
        modeButton.setName("viewBtn");
        exitButton.setName("exitBtn");
        statsButton.setName("statsBtn");

        // 3. App Environment
        setSize (800, 400);
        setAudioChannels (0, 2);
        startTimerHz (60);
        setWantsKeyboardFocus (true);
        drunkLF.setButtonOnColorRange({ juce::Colour(0xffff1493), 0.08f, 0.12f, 0.1f }); // Hot Pink
        drunkLF.setButtonOutlineColorRange({ juce::Colour(0xff00ffff), 0.1f, 0.15f, 0.15f }); // Cyan
    }

    ~MainComponent() override {
        setLookAndFeel(nullptr); // MUST do this to avoid a crash!
        shutdownAudio();
    }

    // --- LOGIC ---
    void playStopButtonClicked() {
        if (transportSource.isPlaying()) transportSource.stop();
        else {
            if (transportSource.hasStreamFinished()) transportSource.setPosition (0.0);
            transportSource.start();
        }
        updateButtonText();
    }

    void updateButtonText() {
        if (transportSource.isPlaying())
            playStopButton.setButtonText (juce::CharPointer_UTF8 ("\xe2\x8f\xb8")); // Pause
        else
            playStopButton.setButtonText (juce::CharPointer_UTF8 ("\xe2\x96\xb6")); // Play
    }

    void openButtonClicked(){
        auto filter = formatManager.getWildcardForAllFormats();
        chooser = std::make_unique<juce::FileChooser> ("Select Audio...",
            juce::File::getSpecialLocation (juce::File::userHomeDirectory), filter);

        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync (flags, [this] (const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.exists()) {
                auto* reader = formatManager.createReaderFor (file);
                if (reader != nullptr) {
                    transportSource.stop();
                    transportSource.setSource (nullptr);
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                    transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                    thumbnail.setSource (new juce::FileInputSource (file));
                    playStopButton.setEnabled (true);
                    readerSource.reset (newSource.release());
                    updateButtonText();
                }
            }
        });
    }

    // --- AUDIO & UI CALLBACKS ---
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override { transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate); }
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override {
        if (readerSource.get() == nullptr) bufferToFill.clearActiveBufferRegion();
        else transportSource.getNextAudioBlock (bufferToFill);
    }
    void releaseResources() override { transportSource.releaseResources(); }

    void timerCallback() override {
        if (!transportSource.isPlaying() && playStopButton.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x8f\xb8"))
            updateButtonText();

        if (showStats) {
            juce::String debugInfo;

            // 1. Samples Finished
            debugInfo << "Samples Loaded: " << thumbnail.getNumSamplesFinished() << "\n";

            // 2. Approximate Peak
            debugInfo << "Approx Peak: " << thumbnail.getApproximatePeak() << "\n";

            // 3. Min/Max for current view
            float minV, maxV;
            thumbnail.getApproximateMinMax(0.0, thumbnail.getTotalLength(), 0, minV, maxV);
            debugInfo << "Min/Max: " << minV << " / " << maxV << "\n";

            // 4. Transport Info
            debugInfo << "Position: " << transportSource.getCurrentPosition() << "s\n";

            statsDisplay.setText(debugInfo, false);
        }

        repaint();
    }
    void changeListenerCallback (juce::ChangeBroadcaster*) override { repaint(); }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colours::black);

        if (thumbnail.getNumChannels() > 0) {
            g.setColour (juce::Colours::deeppink);
            thumbnail.drawChannels (g, waveformBounds, 0.0, thumbnail.getTotalLength(), 1.0f);

            auto audioLength = (float)thumbnail.getTotalLength();
            auto drawPosition = (float)transportSource.getCurrentPosition();
            auto x = (drawPosition / audioLength) * (float)waveformBounds.getWidth() + (float)waveformBounds.getX();

            g.setColour (juce::Colours::white);
            g.drawVerticalLine ((int)x, (float)waveformBounds.getY(), (float)waveformBounds.getBottom());
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        // 1. BOTTOM ROW: Mode and Stats Toggles
        // This row stays constant in both view modes
        auto bottomArea = bounds.removeFromBottom (40).reduced (5);
        modeButton.setBounds (bottomArea.removeFromRight (80));
        bottomArea.removeFromRight (5);
        statsButton.setBounds (bottomArea.removeFromRight (80));

        // 2. TOP ROW: Open, Play (Left) and Exit (Right)
        // This row also stays constant to maintain the "brand" layout
        auto topRow = bounds.removeFromTop (50).reduced (5);

        // Pin Exit to the right
        exitButton.setBounds (topRow.removeFromRight (80));

        // Pin others to the left
        openButton.setBounds (topRow.removeFromLeft (80));
        topRow.removeFromLeft (5);
        playStopButton.setBounds (topRow.removeFromLeft (80));

        // 3. DEBUG STATS AREA: Carve out space if toggled on
        if (showStats)
        {
            statsDisplay.setVisible (true);
            // We take this from the bottom, right above the bottom button row
            statsBounds = bounds.removeFromBottom (100).reduced (10, 5);
            statsDisplay.setBounds (statsBounds);
        }
        else
        {
            statsDisplay.setVisible (false);
        }

        // 4. WAVEFORM LOGIC: Decide the background layer
        if (currentMode == ViewMode::Classic)
        {
            // In Classic, the waveform sits in the remaining center space
            waveformBounds = bounds.reduced (10, 0);

            // Solid visibility
            openButton.setAlpha (1.0f);
            playStopButton.setAlpha (1.0f);
            exitButton.setAlpha (1.0f);
            modeButton.setAlpha (1.0f);
            statsButton.setAlpha (1.0f);
        }
        else
        {
            // In Overlay, the waveform fills the WHOLE window behind the buttons
            waveformBounds = getLocalBounds();

            // Drunk/Sloppy transparency for the buttons
            openButton.setAlpha (0.7f);
            playStopButton.setAlpha (0.7f);
            exitButton.setAlpha (0.7f);
            modeButton.setAlpha (0.7f);
            statsButton.setAlpha (0.7f);
        }
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    juce::TextButton openButton, playStopButton, modeButton, exitButton;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::Rectangle<int> waveformBounds;
    ViewMode currentMode = ViewMode::Classic;

    DrunkDrawnLookAndFeel drunkLF;

    juce::TextButton statsButton;
    juce::TextEditor statsDisplay;
    bool showStats = false;
    juce::Rectangle<int> statsBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};