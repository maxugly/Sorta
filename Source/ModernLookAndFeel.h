#pragma once

#include <JuceHeader.h>
#include "Config.h" // Include Config.h

class ModernLookAndFeel : public juce::LookAndFeel_V4 {
public:
  ModernLookAndFeel() {
    setColour (juce::TextButton::buttonColourId, Config::buttonBaseColour);
    setColour (juce::TextButton::buttonOnColourId, Config::buttonOnColour);
    setColour (juce::TextButton::textColourOffId, Config::buttonTextColour);
    setColour (juce::TextButton::textColourOnId, Config::buttonTextColour); } // Use same text colour for on/off states

  void setBaseOffColor(juce::Colour color) { setColour(juce::TextButton::buttonColourId, color); }
  void setBaseOnColor(juce::Colour color) { setColour(juce::TextButton::buttonOnColourId, color); }
  void setTextColor(juce::Colour color) { setColour(juce::TextButton::textColourOffId, color); setColour(juce::TextButton::textColourOnId, color); }

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
    auto cornerSize = Config::buttonCornerRadius;
    auto outlineThickness = Config::buttonOutlineThickness;
    auto bounds = button.getLocalBounds().toFloat().reduced (outlineThickness / 2.0f); // Reduce by half thickness for outline

    auto baseColour = backgroundColour;
    if (shouldDrawButtonAsHighlighted)
      baseColour = baseColour.brighter (Config::buttonHighlightedBrightnessFactor);
    if (shouldDrawButtonAsDown)
      baseColour = baseColour.darker (Config::buttonPressedDarknessFactor);

    g.setColour (baseColour);
    g.fillRoundedRectangle (bounds, cornerSize);

    g.setColour (Config::buttonOutlineColour);
    g.drawRoundedRectangle (bounds, cornerSize, outlineThickness); }

  juce::Font getTextButtonFont (juce::TextButton& button, int buttonHeight) override {
    juce::Font font = LookAndFeel_V4::getTextButtonFont (button, buttonHeight);
    if (button.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x96\xb6") ||        // Play symbol
      button.getButtonText() == juce::CharPointer_UTF8 ("\xe2\x8f\xb8")) {          // Pause symbol
      font.setHeight (buttonHeight * Config::buttonPlayPauseTextHeightScale); }
    else {
      font.setHeight (buttonHeight * Config::buttonTextHeightScale); }
    return font; }

  void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                      bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
    auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                          : juce::TextButton::textColourOffId));
    g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true); }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernLookAndFeel) };
