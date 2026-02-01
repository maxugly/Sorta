#pragma once

#include <JuceHeader.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour (juce::TextButton::buttonColourId, juce::Colour(0xff3a3a3a));         // Dark Grey
        setColour (juce::TextButton::buttonOnColourId, juce::Colour(0xff00bfff));        // Deep Sky Blue
        setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    }

    void setBaseOffColor(juce::Colour color) { setColour(juce::TextButton::buttonColourId, color); }
    void setBaseOnColor(juce::Colour color) { setColour(juce::TextButton::buttonOnColourId, color); }
    void setTextColor(juce::Colour color) { setColour(juce::TextButton::textColourOffId, color); setColour(juce::TextButton::textColourOnId, color); }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 5.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
        auto baseColour = backgroundColour;

        if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter (0.1f);
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker (0.1f);

        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (juce::Colour(0xff808080)); // Medium Grey Outline
        g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
    }

    juce::Font getTextButtonFont (juce::TextButton& button, int buttonHeight) override
    {
        juce::Font font = LookAndFeel_V4::getTextButtonFont (button, buttonHeight);
        if (button.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x96\xb6") || // Play symbol
            button.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x8f\xb8"))   // Pause symbol
        {
            font.setHeight (buttonHeight * 0.7f); // Scale up to 70% of button height
        } else {
            font.setHeight (buttonHeight * 0.45f); // Default scale for other text
        }
        return font;
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto font = getTextButtonFont (button, button.getHeight());
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId));
        g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernLookAndFeel)
};
