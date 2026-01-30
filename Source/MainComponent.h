#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class MainComponent  : public juce::AudioAppComponent,
                       public juce::ChangeListener,
                       public juce::Timer
{
public:
    enum class ViewMode { Classic, Overlay };

    MainComponent() : thumbnailCache (5),
                      thumbnail (512, formatManager, thumbnailCache)
    {
        formatManager.registerFormat (new juce::WavAudioFormat(), false);
        formatManager.registerFormat (new juce::AiffAudioFormat(), false);
        formatManager.registerFormat (new juce::FlacAudioFormat(), false);
        formatManager.registerFormat (new juce::OggVorbisAudioFormat(), false);
        formatManager.registerFormat (new juce::MP3AudioFormat(), false);

        thumbnail.addChangeListener (this);

        addAndMakeVisible (openButton);
        openButton.setButtonText ("Open");
        openButton.onClick = [this] { openButtonClicked(); };

        addAndMakeVisible (playStopButton);
        updateButtonText();
        playStopButton.onClick = [this] { playStopButtonClicked(); };
        playStopButton.setEnabled (false);

        // Mode Toggle Button
        addAndMakeVisible (modeButton);
        modeButton.setButtonText ("View");
        modeButton.onClick = [this] {
            currentMode = (currentMode == ViewMode::Classic) ? ViewMode::Overlay : ViewMode::Classic;
            resized();
            repaint();
        };

        setSize (800, 400);
        setAudioChannels (0, 2);
        startTimerHz (60);
        setWantsKeyboardFocus (true);
    }

    ~MainComponent() override { shutdownAudio(); }

    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::spaceKey) { playStopButtonClicked(); return true; }
        return false;
    }

    void openButtonClicked()
    {
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

    void playStopButtonClicked()
    {
        if (transportSource.isPlaying()) transportSource.stop();
        else {
            if (transportSource.hasStreamFinished()) transportSource.setPosition (0.0);
            transportSource.start();
        }
        updateButtonText();
    }

    void updateButtonText()
    {
        if (transportSource.isPlaying())
            playStopButton.setButtonText (juce::CharPointer_UTF8 ("\xe2\x8f\xb8")); // Pause
        else
            playStopButton.setButtonText (juce::CharPointer_UTF8 ("\xe2\x96\xb6")); // Play
    }

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override { transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate); }
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override {
        if (readerSource.get() == nullptr) bufferToFill.clearActiveBufferRegion();
        else transportSource.getNextAudioBlock (bufferToFill);
    }
    void releaseResources() override { transportSource.releaseResources(); }

    void timerCallback() override
    {
        if (!transportSource.isPlaying() && playStopButton.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x8f\xb8"))
            updateButtonText();
        repaint();
    }

    void changeListenerCallback (juce::ChangeBroadcaster*) override { repaint(); }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        if (thumbnail.getNumChannels() > 0)
        {
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

        if (currentMode == ViewMode::Classic)
        {
            auto topStrip = bounds.removeFromTop (50).reduced (5);
            openButton.setBounds (topStrip.removeFromLeft (80));
            topStrip.removeFromLeft (5);
            playStopButton.setBounds (topStrip.removeFromLeft (80));
            modeButton.setBounds (topStrip.removeFromRight (80));

            waveformBounds = bounds.reduced (10);

            // Reset opacity for Classic mode
            openButton.setAlpha (1.0f);
            playStopButton.setAlpha (1.0f);
        }
        else
        {
            // Overlay Mode
            waveformBounds = bounds;

            auto overlayArea = bounds.reduced (20);
            openButton.setBounds (overlayArea.removeFromTop (40).removeFromLeft (60));
            overlayArea.removeFromTop (5);
            playStopButton.setBounds (overlayArea.removeFromTop (40).removeFromLeft (60));
            modeButton.setBounds (bounds.removeFromBottom (40).removeFromRight (80).reduced (5));

            // Semi-transparent buttons for Overlay
            openButton.setAlpha (0.7f);
            playStopButton.setAlpha (0.7f);
        }
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    juce::TextButton openButton, playStopButton, modeButton;
    std::unique_ptr<juce::FileChooser> chooser;

    // Fixed the declared scope issue
    juce::Rectangle<int> waveformBounds;
    ViewMode currentMode = ViewMode::Classic;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};