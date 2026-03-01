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
    if (!state.isVisible) return;

    const float x = state.playheadX;
    const int ix = juce::roundToInt(x); // Snap to integer grid for razor-sharp 1px line
    const auto bounds = getLocalBounds();
    const float h = (float)bounds.getHeight();
    const float centerY = (float)bounds.getCentreY();

    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;
    const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
    const float halfBoxWidth = boxWidth / 2.0f;

    const float thickThickness = Config::Layout::Glow::cutBoxOutlineThickness;
    const float medThickness = thickThickness * 0.5f;

    // Y Coordinates with safety clamps for edge cases
    const float capTopY = (float)bounds.getY();
    const float capBottomY = capTopY + boxHeight;
    const float capBottomEndTop = (float)bounds.getBottom() - boxHeight;

    // Exactly +/- 10% distance from the zero-crossing line (20% total span)
    float thinTopY = centerY - (h * 0.10f);
    float thinBottomY = centerY + (h * 0.10f);
    thinTopY = juce::jmax(thinTopY, capBottomY);
    thinBottomY = juce::jmin(thinBottomY, capBottomEndTop);

    // Dynamic split for thick -> med stages
    const float thickBottomY = capBottomY + (thinTopY - capBottomY) * 0.5f;
    const float bottomThickTopY = thinBottomY + (capBottomEndTop - thinBottomY) * 0.5f;

    g.setColour(state.headColor);

    // Caps
    g.fillRect(x - halfBoxWidth, capTopY, boxWidth, boxHeight);
    g.fillRect(x - halfBoxWidth, capBottomEndTop, boxWidth, boxHeight);

    // Thick Pillars
    g.fillRect(x - (thickThickness * 0.5f), capBottomY, thickThickness, thickBottomY - capBottomY);
    g.fillRect(x - (thickThickness * 0.5f), bottomThickTopY, thickThickness, capBottomEndTop - bottomThickTopY);

    // Medium Pillars
    g.fillRect(x - (medThickness * 0.5f), thickBottomY, medThickness, thinTopY - thickBottomY);
    g.fillRect(x - (medThickness * 0.5f), thinBottomY, medThickness, bottomThickTopY - thinBottomY);

    // Razor-Sharp 1-Pixel Center
    g.setColour(state.centerLineColor);
    g.drawVerticalLine(ix, thinTopY, thinBottomY);
}
