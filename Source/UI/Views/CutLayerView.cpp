#include "UI/Views/CutLayerView.h"

#include "Core/WaveformManager.h"
#include "UI/ControlPanel.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "Utils/CoordinateMapper.h"

CutLayerView::CutLayerView(ControlPanel &ownerIn)
    : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

CutLayerView::~CutLayerView() = default;

void CutLayerView::updateState(const CutLayerState& newState) {
    // 1. Full repaint for major structural changes
    if (state.markersVisible != newState.markersVisible ||
        state.audioLength != newState.audioLength ||
        state.channelMode != newState.channelMode ||
        state.inThresholdYTop != newState.inThresholdYTop ||
        state.outThresholdYTop != newState.outThresholdYTop) {
        state = newState;
        repaint();
        return;
    }

    // 2. Dirty Rectangle: Cut In Marker & Left Fade Region
    if (state.inPixelX != newState.inPixelX ||
        state.inMarkerColor != newState.inMarkerColor ||
        state.regionOutlineColor != newState.regionOutlineColor) {
        const float minX = juce::jmin(state.inPixelX, newState.inPixelX);
        const float maxX = juce::jmax(state.inPixelX, newState.inPixelX);
        const float padding = Config::Layout::Glow::cutMarkerBoxWidth + Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        repaint((int)(minX - padding), 0, (int)(maxX - minX + padding * 2.0f), getHeight());
    }

    // 3. Dirty Rectangle: Cut Out Marker & Right Fade Region
    if (state.outPixelX != newState.outPixelX ||
        state.outMarkerColor != newState.outMarkerColor) {
        const float minX = juce::jmin(state.outPixelX, newState.outPixelX);
        const float maxX = juce::jmax(state.outPixelX, newState.outPixelX);
        const float padding = Config::Layout::Glow::cutMarkerBoxWidth + Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        repaint((int)(minX - padding), 0, (int)(maxX - minX + padding * 2.0f), getHeight());
    }

    state = newState;
}

void CutLayerView::paint(juce::Graphics &g) {
    if (!state.markersVisible)
        return;

    if (state.audioLength <= 0.0f)
        return;

    drawThresholds(g);
    drawFadeRegions(g);
    drawMarkersAndRegion(g);
}

void CutLayerView::drawThresholds(juce::Graphics& g) {
    const auto bounds = getLocalBounds();
    auto drawThresholdVisualisation = [&](float xPos, float topThresholdY, float bottomThresholdY, juce::Colour color) {
        const float halfThresholdLineWidth = Config::Animation::thresholdLineWidth / 2.0f;
        float lineStartX = xPos - halfThresholdLineWidth;
        float lineEndX = xPos + halfThresholdLineWidth;

        lineStartX = juce::jmax(lineStartX, (float)bounds.getX());
        lineEndX = juce::jmin(lineEndX, (float)bounds.getRight());

        g.setColour(color);
        g.drawHorizontalLine((int)topThresholdY, lineStartX, lineEndX);
        g.drawHorizontalLine((int)bottomThresholdY, lineStartX, lineEndX);
    };

    drawThresholdVisualisation(state.inPixelX, state.inThresholdYTop, state.inThresholdYBottom, state.inThresholdColor);
    drawThresholdVisualisation(state.outPixelX, state.outThresholdYTop, state.outThresholdYBottom, state.outThresholdColor);
}

void CutLayerView::drawFadeRegions(juce::Graphics& g) {
    g.setColour(Config::Colors::solidBlack);
    const float actualInX = state.actualInX;
    const float actualOutX = state.actualOutX;
    const auto bounds = getLocalBounds().toFloat();
    
    // Blackout left of In marker
    if (actualInX > bounds.getX()) {
        g.fillRect(bounds.getX(), bounds.getY(), actualInX - bounds.getX(), bounds.getHeight());
    }
    // Blackout right of Out marker
    if (actualOutX < bounds.getRight()) {
        g.fillRect(actualOutX, bounds.getY(), bounds.getRight() - actualOutX, bounds.getHeight());
    }
}

void CutLayerView::drawMarkersAndRegion(juce::Graphics& g) {
    const auto bounds = getLocalBounds();
    const float actualInX = state.actualInX;
    const float actualOutX = state.actualOutX;
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;

    auto drawCutMarker = [&](float x, juce::Colour markerColor, float thickness, bool shouldPulse) {
        juce::ignoreUnused(shouldPulse);
        const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
        const float halfBoxWidth = boxWidth / 2.0f;
        const float pillarHeight = (float)bounds.getHeight() * 0.15f;

        g.setColour(markerColor);
        
        // Top/Bottom Squares
        g.fillRect(x - halfBoxWidth, (float)bounds.getY(), boxWidth, boxHeight);
        g.fillRect(x - halfBoxWidth, (float)bounds.getBottom() - boxHeight, boxWidth, boxHeight);

        // Thick Pillars
        g.fillRect(x - (thickness * 0.5f), (float)bounds.getY() + boxHeight, thickness, pillarHeight);
        g.fillRect(x - (thickness * 0.5f), (float)bounds.getBottom() - boxHeight - pillarHeight, thickness, pillarHeight);

        // Thin Center Line
        const float thinThickness = Config::Layout::Glow::cutMarkerWidthThin;
        g.fillRect(x - (thinThickness * 0.5f), 
                   (float)bounds.getY() + boxHeight + pillarHeight, 
                   thinThickness, 
                   (float)bounds.getHeight() - (2.0f * (boxHeight + pillarHeight)));
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
