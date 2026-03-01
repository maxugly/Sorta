#include "UI/Views/DirectoryRoutingView.h"
#include "Utils/Config.h"

void DirectoryRoutingView::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    if (bounds.isEmpty()) return;

    // Divide the vertical space perfectly into 10 destination slots
    const float slotHeight = (float)bounds.getHeight() / 10.0f;

    for (int i = 0; i < 10; ++i) {
        auto slot = bounds.withY((int)(i * slotHeight)).withHeight((int)slotHeight);
        
        // Draw alternating backgrounds for visual distinction
        if (i % 2 == 0) {
            g.fillAll(Config::Colors::Button::disabledBackground);
        } else {
            g.fillAll(Config::Colors::Button::disabledBackground.darker(0.2f));
        }
        
        // Draw slot dividers
        g.setColour(Config::Colors::Button::outline);
        g.drawRect(slot, 1);
        
        // Label them 1-9, terminating in 0
        juce::String label = "Folder " + juce::String(i == 9 ? 0 : i + 1);
        g.setColour(Config::Colors::Button::text);
        g.setFont(juce::Font(juce::FontOptions((float)Config::Layout::Text::mouseCursorSize)));
        g.drawText(label, slot, juce::Justification::centred, false);
    }
}

void DirectoryRoutingView::resized() {
    // Passive view: no child components to lay out yet
}
