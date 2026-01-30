#pragma once

#include "DrunkDrawnLookAndFeel.h"

//==============================================================================
/**
    Example component demonstrating the DrunkDrawnLookAndFeel
*/
class ExampleAudioSorterUI : public juce::Component
{
public:
    ExampleAudioSorterUI()
    {
        // Setup the custom look and feel with your color scheme
        customLF.setButtonOffColorRange({
            juce::Colour(0xff3a3a3a),  // Dark grey base
            0.05f, 0.1f, 0.15f         // Subtle variations
        });
        
        customLF.setButtonOnColorRange({
            juce::Colour(0xffff1493),  // Hot pink base (matching your waveform)
            0.08f, 0.12f, 0.1f         // Pink variations
        });
        
        customLF.setButtonOutlineColorRange({
            juce::Colour(0xff00ffff),  // Cyan/teal base (matching your border)
            0.1f, 0.15f, 0.15f         // Color variations
        });
        
        customLF.setTextColorRange({
            juce::Colours::white,
            0.0f, 0.0f, 0.08f          // Mostly white with slight brightness variation
        });
        
        // Adjust wobble settings - play with these values!
        DrunkDrawnLookAndFeel::WobbleSettings wobble;
        wobble.edgeWobble = 4.0f;              // Increase for more drunk effect
        wobble.cornerRadiusVariation = 0.4f;   // 0.0 = uniform corners, 1.0 = wild
        wobble.controlPointVariation = 0.6f;   // How drunk the curves are
        wobble.segmentsPerSide = 10;           // More = more detailed wobble
        wobble.strokeWidthVariation = 0.3f;    // Variation in outline thickness
        wobble.rotationVariation = 2.0f;       // Slight rotation (degrees)
        customLF.setWobbleSettings(wobble);
        
        // Create buttons - Single click buttons
        openButton.setButtonText("Open");
        openButton.setName("openBtn");  // Unique name for consistent randomization
        addAndMakeVisible(openButton);
        
        exitButton.setButtonText("Exit");
        exitButton.setName("exitBtn");
        exitButton.onClick = [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); };
        addAndMakeVisible(exitButton);
        
        // Toggle buttons
        playPauseButton.setButtonText("▶");
        playPauseButton.setName("playBtn");
        playPauseButton.setClickingTogglesState(true);
        addAndMakeVisible(playPauseButton);
        
        viewButton.setButtonText("View");
        viewButton.setName("viewBtn");
        viewButton.setClickingTogglesState(true);
        addAndMakeVisible(viewButton);
        
        loopButton.setButtonText("Loop");
        loopButton.setName("loopBtn");
        loopButton.setClickingTogglesState(true);
        addAndMakeVisible(loopButton);
        
        monitorButton.setButtonText("Monitor FS");
        monitorButton.setName("monitorBtn");
        monitorButton.setClickingTogglesState(true);
        addAndMakeVisible(monitorButton);
        
        copyMoveButton.setButtonText("Copy/Move");
        copyMoveButton.setName("copyMoveBtn");
        copyMoveButton.setClickingTogglesState(true);
        addAndMakeVisible(copyMoveButton);
        
        // Sort buttons (0-9 keybinds)
        for (int i = 0; i < 10; ++i)
        {
            auto* btn = new juce::TextButton();
            btn->setButtonText("Sort " + juce::String(i));
            btn->setName("sortBtn" + juce::String(i));
            sortButtons.add(btn);
            addAndMakeVisible(btn);
        }
        
        // Text editor example
        textEditor.setText("audio_file.wav");
        textEditor.setName("fileNameEditor");
        addAndMakeVisible(textEditor);
        
        // Apply the custom look and feel to all components
        setLookAndFeel(&customLF);
        
        setSize(800, 600);
    }
    
    ~ExampleAudioSorterUI() override
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dark background like your screenshots
        g.fillAll(juce::Colour(0xff1a1a1a));
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        
        // Top row - main controls
        auto topRow = area.removeFromTop(50);
        openButton.setBounds(topRow.removeFromLeft(80).reduced(2));
        topRow.removeFromLeft(5);
        playPauseButton.setBounds(topRow.removeFromLeft(60).reduced(2));
        topRow.removeFromLeft(5);
        exitButton.setBounds(topRow.withWidth(80).withRightX(area.getRight()).reduced(2));
        
        area.removeFromTop(10);
        
        // Second row - toggles
        auto toggleRow = area.removeFromTop(40);
        viewButton.setBounds(toggleRow.removeFromLeft(100).reduced(2));
        toggleRow.removeFromLeft(5);
        loopButton.setBounds(toggleRow.removeFromLeft(100).reduced(2));
        toggleRow.removeFromLeft(5);
        monitorButton.setBounds(toggleRow.removeFromLeft(130).reduced(2));
        toggleRow.removeFromLeft(5);
        copyMoveButton.setBounds(toggleRow.removeFromLeft(130).reduced(2));
        
        area.removeFromTop(10);
        
        // Sort buttons (two rows of 5)
        auto sortArea = area.removeFromTop(100);
        auto sortRow1 = sortArea.removeFromTop(45);
        auto sortRow2 = sortArea;
        
        for (int i = 0; i < 5; ++i)
        {
            sortButtons[i]->setBounds(sortRow1.removeFromLeft(140).reduced(2));
            sortRow1.removeFromLeft(5);
        }
        
        for (int i = 5; i < 10; ++i)
        {
            sortButtons[i]->setBounds(sortRow2.removeFromLeft(140).reduced(2));
            sortRow2.removeFromLeft(5);
        }
        
        area.removeFromTop(10);
        
        // Text editor
        textEditor.setBounds(area.removeFromTop(35).reduced(2));
    }

private:
    DrunkDrawnLookAndFeel customLF;
    
    juce::TextButton openButton;
    juce::TextButton exitButton;
    juce::TextButton playPauseButton;
    juce::TextButton viewButton;
    juce::TextButton loopButton;
    juce::TextButton monitorButton;
    juce::TextButton copyMoveButton;
    
    juce::OwnedArray<juce::TextButton> sortButtons;
    
    juce::TextEditor textEditor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExampleAudioSorterUI)
};
