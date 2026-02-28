#include "UI/Views/PlaybackCursorView.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

PlaybackCursorView::PlaybackCursorView(ControlPanel &ownerPanel) : owner(ownerPanel) {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

PlaybackCursorView::~PlaybackCursorView() = default;

void PlaybackCursorView::updateState(const PlaybackCursorViewState& newState) {
    if (!newState.isVisible) {
        if (state.isVisible) {
            state.isVisible = false;
            setVisible(false);
        }
        return;
    }

    if (!state.isVisible) {
        state.isVisible = true;
        setVisible(true);
    }

    const int currentX = juce::roundToInt(newState.playheadX);

    if (currentX != lastCursorX || state.centerLineColor != newState.centerLineColor || state.headColor != newState.headColor) {
        const int glowWidth = 4; // Padding for the marker caps
        if (lastCursorX >= 0)
            repaint(lastCursorX - glowWidth, 0, glowWidth * 2, getHeight());

        repaint(currentX - glowWidth, 0, glowWidth * 2, getHeight());

        lastCursorX = currentX;
    }

    state = newState;
}

void PlaybackCursorView::paint(juce::Graphics &g) {
    if (!state.isVisible)
        return;

    const int x = juce::roundToInt(state.playheadX);
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;
    const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
    const float halfBoxWidth = boxWidth / 2.0f;

    // Marker Caps
    g.setColour(state.headColor);
    g.fillRect((float)x - halfBoxWidth, 0.0f, boxWidth, boxHeight);
    g.fillRect((float)x - halfBoxWidth, (float)getHeight() - boxHeight, boxWidth, boxHeight);

    // Center Line
    g.setColour(state.centerLineColor);
    g.drawVerticalLine(x, boxHeight, (float)getHeight() - boxHeight);
}
