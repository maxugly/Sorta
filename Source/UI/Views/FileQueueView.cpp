#include "UI/Views/FileQueueView.h"
#include "Utils/Config.h"

// --- FileQueueContentView ---
FileQueueContentView::FileQueueContentView() { setInterceptsMouseClicks(false, false); }

void FileQueueContentView::updateState(const FileQueueViewState& newState) {
    state = newState;
    // Dynamically resize the canvas height based on the number of files (22px per row)
    setSize(getParentWidth(), juce::jmax(getHeight(), (int)state.upcomingFiles.size() * 22));
    repaint();
}

void FileQueueContentView::paint(juce::Graphics& g) {
    g.setFont((float)Config::Layout::Text::mouseCursorSize);

    int yOffset = 0;
    const int rowHeight = 22;
    const float w = (float)getWidth();

    for (size_t i = 0; i < state.upcomingFiles.size(); ++i) {
        // Alternating row backgrounds
        g.setColour(i % 2 == 0 ? Config::Colors::Button::disabledBackground : Config::Colors::Button::disabledBackgroundAlt);
        g.fillRect(0, yOffset, (int)w, rowHeight);

        g.setColour(Config::Colors::statsText);
        const auto& item = state.upcomingFiles[i];

        // Draw columns: File (50%), Size (20%), Modified (30%)
        g.drawText(item.filename, 10, yOffset, (int)(w * 0.5f) - 10, rowHeight, juce::Justification::centredLeft, true);
        g.drawText(item.sizeStr, (int)(w * 0.5f), yOffset, (int)(w * 0.2f), rowHeight, juce::Justification::centredLeft, true);
        g.drawText(item.modifiedStr, (int)(w * 0.7f), yOffset, (int)(w * 0.3f) - 10, rowHeight, juce::Justification::centredRight, true);

        yOffset += rowHeight;
    }
}

// --- FileQueueView (The Shell) ---
FileQueueView::FileQueueView() {
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&content, false);
    viewport.setScrollBarsShown(true, false); // Vertical only
}

FileQueueView::~FileQueueView() = default;

void FileQueueView::updateState(const FileQueueViewState& newState) {
    content.updateState(newState);
    repaint();
}

void FileQueueView::resized() {
    auto bounds = getLocalBounds();
    // Leave room at the top for the header row
    viewport.setBounds(bounds.withTrimmedTop(30).reduced(Config::UI::ButtonOutlineThickness));
    content.setSize(viewport.getMaximumVisibleWidth(), content.getHeight());
}

void FileQueueView::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(Config::Colors::textEditorBackground);
    g.fillRoundedRectangle(bounds, Config::UI::ButtonCornerSize);
    g.setColour(Config::Colors::Button::outline);
    g.drawRoundedRectangle(bounds, Config::UI::ButtonCornerSize, Config::UI::ButtonOutlineThickness);

    // Draw Headers
    g.setColour(Config::Colors::Button::text);
    g.setFont((float)Config::Layout::Text::mouseCursorSize);
    g.setFont(g.getCurrentFont().boldened());

    const float w = bounds.getWidth() - (Config::UI::ButtonOutlineThickness * 2);
    g.drawText(Config::Labels::queueHeaderFile, 10, 5, (int)(w * 0.5f) - 10, 20, juce::Justification::centredLeft, true);
    g.drawText(Config::Labels::queueHeaderSize, (int)(w * 0.5f), 5, (int)(w * 0.2f), 20, juce::Justification::centredLeft, true);
    g.drawText(Config::Labels::queueHeaderDate, (int)(w * 0.7f), 5, (int)(w * 0.3f) - 20, 20, juce::Justification::centredRight, true);

    g.drawLine(bounds.getX(), 30.0f, bounds.getRight(), 30.0f, 1.0f);
}
