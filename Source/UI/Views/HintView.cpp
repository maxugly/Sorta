#include "UI/Views/HintView.h"
#include "Core/AppEnums.h"
#include "Utils/Config.h"

HintView::HintView() { 
    setInterceptsMouseClicks(false, false); 
    currentHint = Config::Labels::defaultHint;
}

void HintView::setHint(const juce::String& text) {
    if (currentHint != text) {
        currentHint = text;
        repaint();
    }
}

void HintView::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(Config::UI::ButtonOutlineThickness / 2.0f);
    auto cornerSize = Config::UI::ButtonCornerSize;

    // 1. Determine Corner Shapes based on GroupPosition
    const int pos = getProperties().getWithDefault("GroupPosition", (int)AppEnums::GroupPosition::Right);
    const bool roundLeft = (pos == (int)AppEnums::GroupPosition::Left || pos == (int)AppEnums::GroupPosition::Alone);
    const bool roundRight = (pos == (int)AppEnums::GroupPosition::Right || pos == (int)AppEnums::GroupPosition::Alone);

    juce::Path p;
    p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          cornerSize, cornerSize, roundLeft, roundRight, roundLeft, roundRight);

    g.setColour(Config::Colors::Button::base);
    g.fillPath(p);

    g.setColour(Config::Colors::Button::outline);
    g.strokePath(p, juce::PathStrokeType(Config::UI::ButtonOutlineThickness));
    
    // Draw the text
    g.setColour(Config::Colors::HintVox::text);
    g.setFont((float)Config::Layout::Text::mouseCursorSize);
    g.drawText(currentHint, getLocalBounds().reduced(10, 0), juce::Justification::centredLeft, true);
}
