#ifndef AUDIOFILER_MODERNLOOKANDFEEL_H
#define AUDIOFILER_MODERNLOOKANDFEEL_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Utils/Config.h"

class ModernLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    ModernLookAndFeel() {
        setColour(juce::TextButton::buttonColourId, Config::Colors::Button::base);

        setColour(juce::TextButton::buttonOnColourId, Config::Colors::Button::on);

        setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);

        setColour(juce::TextButton::textColourOnId, Config::Colors::Button::text);
    }

    void setBaseOffColor(juce::Colour color) {
        setColour(juce::TextButton::buttonColourId, color);
    }

    void setBaseOnColor(juce::Colour color) {
        setColour(juce::TextButton::buttonOnColourId, color);
    }

    void setTextColor(juce::Colour color) {
        setColour(juce::TextButton::textColourOffId, color);
        setColour(juce::TextButton::textColourOnId, color);
    }

    void drawButtonBackground(juce::Graphics &g, juce::Button &button,
                              const juce::Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override {
        auto cornerSize = Config::UI::ButtonCornerSize;
        auto outlineThickness = Config::UI::ButtonOutlineThickness;
        auto bounds = button.getLocalBounds().toFloat().reduced(outlineThickness / 2.0f);

        auto groupPos = (AppEnums::GroupPosition)(int)button.getProperties().getWithDefault(
            "GroupPosition", (int)AppEnums::GroupPosition::Alone);

        bool tl = false, tr = false, bl = false, br = false;
        switch (groupPos) {
        case AppEnums::GroupPosition::Alone:
            tl = tr = bl = br = true;
            break;
        case AppEnums::GroupPosition::Left:
            tl = bl = true;
            break;
        case AppEnums::GroupPosition::Right:
            tr = br = true;
            break;
        case AppEnums::GroupPosition::Middle:
            break;
        }

        juce::Path p;
        p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                              cornerSize, cornerSize, tl, tr, bl, br);

        juce::Colour currentBackgroundColour;
        if (!button.isEnabled()) {
            currentBackgroundColour = Config::Colors::Button::disabledBackground;
        } else {
            currentBackgroundColour = backgroundColour;
            if (shouldDrawButtonAsHighlighted)
                currentBackgroundColour = currentBackgroundColour.brighter(
                    Config::Animation::buttonHighlightedBrightness);
            if (shouldDrawButtonAsDown)
                currentBackgroundColour =
                    currentBackgroundColour.darker(Config::Animation::buttonPressedDarkness);
        }

        g.setColour(currentBackgroundColour);
        g.fillPath(p);

        g.setColour(Config::Colors::Button::outline);
        g.strokePath(p, juce::PathStrokeType(outlineThickness));
    }

    juce::Font getTextButtonFont(juce::TextButton &button, int buttonHeight) override {
        juce::Font font = LookAndFeel_V4::getTextButtonFont(button, buttonHeight);
        if (button.getButtonText() == Config::Labels::playButton ||
            button.getButtonText() == Config::Labels::stopButton) {
            font.setHeight(buttonHeight * Config::Layout::Text::buttonPlayPauseHeightScale);
        } else {
            font.setHeight(buttonHeight * Config::Layout::Text::buttonHeightScale);
        }

        if (button.getButtonText() == "EXIT") {
            font.setBold(true);
        }

        return font;
    }

    void drawButtonText(juce::Graphics &g, juce::TextButton &button,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);

        juce::Colour textColourToUse;
        if (!button.isEnabled()) {
            textColourToUse = Config::Colors::Button::disabledText;
        } else {
            textColourToUse =
                button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                          : juce::TextButton::textColourOffId);
        }
        g.setColour(textColourToUse);
        g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred,
                   true);
    }

    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox &box) override {
        auto cornerSize = Config::UI::ButtonCornerSize;
        auto outlineThickness = Config::UI::ButtonOutlineThickness;
        auto bounds = box.getLocalBounds().toFloat().reduced(outlineThickness / 2.0f);

        auto groupPos = (AppEnums::GroupPosition)(int)box.getProperties().getWithDefault(
            "GroupPosition", (int)AppEnums::GroupPosition::Alone);

        bool tl = false, tr = false, bl = false, br = false;
        switch (groupPos) {
        case AppEnums::GroupPosition::Alone:  tl = tr = bl = br = true; break;
        case AppEnums::GroupPosition::Left:   tl = bl = true; break;
        case AppEnums::GroupPosition::Right:  tr = br = true; break;
        case AppEnums::GroupPosition::Middle: break;
        }

        juce::Path p;
        p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                              cornerSize, cornerSize, tl, tr, bl, br);

        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillPath(p);

        g.setColour(Config::Colors::Button::outline);
        g.strokePath(p, juce::PathStrokeType(outlineThickness));

        const float arrowX = (float)buttonX + (float)buttonW * 0.5f;
        const float arrowY = (float)buttonY + (float)buttonH * 0.5f;
        const float arrowW = (float)juce::jmin(buttonW, buttonH) * 0.5f;

        juce::Path arrowPath;
        arrowPath.addTriangle(0.0f, 0.0f, arrowW * 0.5f, arrowW * 0.4f, -arrowW * 0.5f, arrowW * 0.4f);
        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha(box.isEnabled() ? 1.0f : 0.5f));
        g.fillPath(arrowPath, juce::AffineTransform::translation(arrowX, arrowY - arrowW * 0.2f));
    }

    void fillTextEditorBackground(juce::Graphics &g, int width, int height,
                                  juce::TextEditor &textEditor) override {
        auto cornerSize = Config::UI::ButtonCornerSize;
        auto outlineThickness = Config::UI::ButtonOutlineThickness;
        
        auto groupPos = (AppEnums::GroupPosition)(int)textEditor.getProperties().getWithDefault(
            "GroupPosition", (int)AppEnums::GroupPosition::Alone);

        bool tl = false, tr = false, bl = false, br = false;
        switch (groupPos) {
        case AppEnums::GroupPosition::Alone:  tl = tr = bl = br = true; break;
        case AppEnums::GroupPosition::Left:   tl = bl = true; break;
        case AppEnums::GroupPosition::Right:  tr = br = true; break;
        case AppEnums::GroupPosition::Middle: break;
        }

        juce::Path p;
        auto bounds = textEditor.getLocalBounds().toFloat().reduced(outlineThickness / 2.0f);
        p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                              cornerSize, cornerSize, tl, tr, bl, br);

        if (!textEditor.isEnabled()) {
            g.setColour(Config::Colors::Button::disabledBackground);
        } else {
            g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
        }
        g.fillPath(p);
    }

    void drawTextEditorOutline(juce::Graphics &g, int width, int height,
                               juce::TextEditor &textEditor) override {
        auto cornerSize = Config::UI::ButtonCornerSize;
        auto outlineThickness = Config::UI::ButtonOutlineThickness;
        
        auto groupPos = (AppEnums::GroupPosition)(int)textEditor.getProperties().getWithDefault(
            "GroupPosition", (int)AppEnums::GroupPosition::Alone);

        bool tl = false, tr = false, bl = false, br = false;
        switch (groupPos) {
        case AppEnums::GroupPosition::Alone:  tl = tr = bl = br = true; break;
        case AppEnums::GroupPosition::Left:   tl = bl = true; break;
        case AppEnums::GroupPosition::Right:  tr = br = true; break;
        case AppEnums::GroupPosition::Middle: break;
        }

        juce::Path p;
        auto bounds = textEditor.getLocalBounds().toFloat().reduced(outlineThickness / 2.0f);
        p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                              cornerSize, cornerSize, tl, tr, bl, br);

        if (!textEditor.isEnabled()) {
            g.setColour(Config::Colors::Button::disabledText);
        } else {
            g.setColour(textEditor.findColour(juce::TextEditor::outlineColourId));
        }
        g.strokePath(p, juce::PathStrokeType(outlineThickness));
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernLookAndFeel)
};

#endif
