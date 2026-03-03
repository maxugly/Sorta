#include "UI/Views/DirectoryRoutingView.h"
#include "Utils/Config.h"

DirectoryRoutingView::DirectoryRoutingView() {
    setInterceptsMouseClicks(false, false);
}

void DirectoryRoutingView::updateState(const DirectoryRoutingViewState& newState) {
    state = newState;
    repaint();
}

void DirectoryRoutingView::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(Config::Colors::textEditorBackground);
    g.fillRoundedRectangle(bounds, Config::UI::ButtonCornerSize);

    g.setFont((float)Config::Layout::Text::mouseCursorSize);
    const float slotHeight = bounds.getHeight() / 10.0f;

    for (int i = 0; i < 10; ++i) {
        juce::Rectangle<float> slotBounds(bounds.getX(), bounds.getY() + (i * slotHeight), bounds.getWidth(), slotHeight);

        // Highlight logic for visual feedback
        if (i == state.lastRoutedIndex) {
            g.setColour(Config::Colors::Button::highlightedBackground);
            g.fillRect(slotBounds);
        }

        // Draw Key Label (1-9, then 0)
        juce::String keyLabel = (i == 9) ? "0" : juce::String(i + 1);
        g.setColour(Config::Colors::Button::text);
        g.drawText("[" + keyLabel + "]", slotBounds.withWidth(30).toNearestInt(), juce::Justification::centred, false);

        // Draw Destination Path
        g.setColour(Config::Colors::statsText);
        juce::String pathText = state.destinations[i].isEmpty() ? "Unassigned" : state.destinations[i];
        g.drawText(pathText, slotBounds.withTrimmedLeft(40).toNearestInt(), juce::Justification::centredLeft, true);

        // Draw separator
        g.setColour(Config::Colors::Button::outline);
        g.drawLine(slotBounds.getX(), slotBounds.getBottom(), slotBounds.getRight(), slotBounds.getBottom(), 1.0f);
    }
}
