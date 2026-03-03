#include "UI/Views/CutMarkersView.h"

#include "Utils/Config.h"

CutMarkersView::CutMarkersView() {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

CutMarkersView::~CutMarkersView() = default;

void CutMarkersView::updateState(const CutLayerState& newState) {
    if (state.markersVisible != newState.markersVisible ||
        state.audioLength != newState.audioLength ||
        state.channelMode != newState.channelMode ||
        state.inThresholdYTop != newState.inThresholdYTop ||
        state.outThresholdYTop != newState.outThresholdYTop) {
        state = newState;
        repaint();
        return;
    }

    if (state.inPixelX != newState.inPixelX ||
        state.inMarkerColor != newState.inMarkerColor ||
        state.regionOutlineColor != newState.regionOutlineColor) {
        const float minX = juce::jmin(state.inPixelX, newState.inPixelX);
        const float maxX = juce::jmax(state.inPixelX, newState.inPixelX);
        const float padding = Config::Layout::Glow::cutMarkerBoxWidth + Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        repaint((int)(minX - padding), 0, (int)(maxX - minX + padding * 2.0f), getHeight());
    }

    if (state.outPixelX != newState.outPixelX ||
        state.outMarkerColor != newState.outMarkerColor) {
        const float minX = juce::jmin(state.outPixelX, newState.outPixelX);
        const float maxX = juce::jmax(state.outPixelX, newState.outPixelX);
        const float padding = Config::Layout::Glow::cutMarkerBoxWidth + Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        repaint((int)(minX - padding), 0, (int)(maxX - minX + padding * 2.0f), getHeight());
    }

    state = newState;
}

void CutMarkersView::paint(juce::Graphics &g) {
    if (!state.markersVisible)
        return;

    if (state.audioLength <= 0.0f)
        return;

    const auto bounds = getLocalBounds();
    const float actualInX = state.actualInX;
    const float actualOutX = state.actualOutX;
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;

    auto drawCutMarker = [&](float x, juce::Colour markerColor, float thickness, bool shouldPulse) {
        juce::ignoreUnused(shouldPulse);
        const int ix = juce::roundToInt(x);
        const float h = (float)bounds.getHeight();
        const float centerY = (float)bounds.getCentreY();

        const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;
        const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
        const float halfBoxWidth = boxWidth / 2.0f;

        const float thickThickness = thickness; 
        const float medThickness = thickness * 0.5f;

        const float capTopY = (float)bounds.getY();
        const float capBottomY = capTopY + boxHeight;
        const float capBottomEndTop = (float)bounds.getBottom() - boxHeight;

        float thinTopY = centerY - (h * 0.10f);
        float thinBottomY = centerY + (h * 0.10f);
        thinTopY = juce::jmax(thinTopY, capBottomY);
        thinBottomY = juce::jmin(thinBottomY, capBottomEndTop);

        const float thickBottomY = capBottomY + (thinTopY - capBottomY) * 0.5f;
        const float bottomThickTopY = thinBottomY + (capBottomEndTop - thinBottomY) * 0.5f;

        g.setColour(markerColor);

        g.fillRect(x - halfBoxWidth, capTopY, boxWidth, boxHeight);
        g.fillRect(x - halfBoxWidth, capBottomEndTop, boxWidth, boxHeight);

        g.fillRect(x - (thickThickness * 0.5f), capBottomY, thickThickness, thickBottomY - capBottomY);
        g.fillRect(x - (thickThickness * 0.5f), bottomThickTopY, thickThickness, capBottomEndTop - bottomThickTopY);

        g.fillRect(x - (medThickness * 0.5f), thickBottomY, medThickness, thinTopY - thickBottomY);
        g.fillRect(x - (medThickness * 0.5f), thinBottomY, medThickness, bottomThickTopY - thinBottomY);

        g.drawVerticalLine(ix, thinTopY, thinBottomY);
    };

    drawCutMarker(state.inPixelX, state.inMarkerColor, state.inMarkerThickness, state.inMarkerShouldPulse);
    drawCutMarker(state.outPixelX, state.outMarkerColor, state.outMarkerThickness, state.outMarkerShouldPulse);

    const float halfBoxWidth = Config::Layout::Glow::cutMarkerBoxWidth / 2.0f;
    const float startX = actualInX + halfBoxWidth;
    const float endX = actualOutX - halfBoxWidth;

    if (startX < endX) {
        const float hThickness = state.regionOutlineThickness;
        const float w = endX - startX;

        g.setColour(state.regionOutlineColor);
        g.fillRect(startX, (float)bounds.getY(), w, hThickness);
        g.fillRect(startX, (float)bounds.getY() + boxHeight - hThickness, w, hThickness);
        g.fillRect(startX, (float)bounds.getBottom() - hThickness, w, hThickness);
        g.fillRect(startX, (float)bounds.getBottom() - boxHeight, w, hThickness);
    }
}
