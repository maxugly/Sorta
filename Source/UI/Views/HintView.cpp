#include "UI/Views/HintView.h"
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
    
    // Draw Button Background (GroupPosition::Right - sharp left, rounded right)
    juce::Path p;
    p.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                          cornerSize, cornerSize, false, true, false, true);
    
    g.setColour(Config::Colors::Button::base);
    g.fillPath(p);
    
    g.setColour(Config::Colors::Button::outline);
    g.strokePath(p, juce::PathStrokeType(Config::UI::ButtonOutlineThickness));
    
    // Draw the text
    g.setColour(Config::Colors::HintVox::text);
    g.setFont((float)Config::Layout::Text::mouseCursorSize);
    g.drawText(currentHint, getLocalBounds().reduced(10, 0), juce::Justification::centredLeft, true);
}
