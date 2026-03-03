#include "UI/Views/FileQueueView.h"
#include "Utils/Config.h"

FileQueueView::FileQueueView() {
    setInterceptsMouseClicks(false, false);
}

FileQueueView::~FileQueueView() = default;

void FileQueueView::updateState(const FileQueueViewState& newState) {
    state = newState;
    repaint();
}

void FileQueueView::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(Config::Colors::textEditorBackground);
    g.fillRoundedRectangle(bounds, Config::UI::ButtonCornerSize);
    g.setColour(Config::Colors::Button::outline);
    g.drawRoundedRectangle(bounds, Config::UI::ButtonCornerSize, Config::UI::ButtonOutlineThickness);

    g.setColour(Config::Colors::statsText);
    g.setFont((float)Config::Layout::Text::mouseCursorSize);

    if (state.upcomingFiles.empty()) {
        g.drawText("Queue Empty", getLocalBounds(), juce::Justification::centred, false);
        return;
    }

    int yOffset = 10;
    for (const auto& file : state.upcomingFiles) {
        g.drawText(file, 10, yOffset, getWidth() - 20, 20, juce::Justification::left, true);
        yOffset += 22;
        if (yOffset > getHeight() - 20) break;
    }
}
