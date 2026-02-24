

#include "UI/Views/CutLayerView.h"

#include "Core/SessionState.h"
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
    state = newState;
    repaint();
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
    auto drawThresholdVisualisation = [&](float xPos, float topThresholdY, float bottomThresholdY) {
        const float halfThresholdLineWidth = Config::Animation::thresholdLineWidth / 2.0f;
        float lineStartX = xPos - halfThresholdLineWidth;
        float lineEndX = xPos + halfThresholdLineWidth;

        lineStartX = juce::jmax(lineStartX, (float)bounds.getX());
        lineEndX = juce::jmin(lineEndX, (float)bounds.getRight());

        g.setColour(Config::Colors::thresholdLine);
        g.drawHorizontalLine((int)topThresholdY, lineStartX, lineEndX);
        g.drawHorizontalLine((int)bottomThresholdY, lineStartX, lineEndX);
    };

    drawThresholdVisualisation(state.inPixelX, state.inThresholdYTop, state.inThresholdYBottom);
    drawThresholdVisualisation(state.outPixelX, state.outThresholdYTop, state.outThresholdYBottom);
}

void CutLayerView::drawFadeRegions(juce::Graphics& g) {
    const auto bounds = getLocalBounds().toFloat();
    g.setColour(Config::Colors::solidBlack);
    g.fillRect(bounds.withWidth(state.actualInX - bounds.getX()));
    g.fillRect(bounds.withLeft(state.actualOutX));
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

        // Top/Bottom Boxes
        g.setColour(markerColor);
        g.drawRect(x - halfBoxWidth, (float)bounds.getY(), boxWidth, boxHeight, thickness);
        g.drawRect(x - halfBoxWidth, (float)bounds.getBottom() - boxHeight, boxWidth, boxHeight,
                   thickness);

        // Precision Pillars (Thick segments at top and bottom)
        g.fillRect(x - thickness * 0.5f, (float)bounds.getY() + boxHeight, thickness, pillarHeight);
        g.fillRect(x - thickness * 0.5f, (float)bounds.getBottom() - boxHeight - pillarHeight, thickness, pillarHeight);

        // Thin Center Line (Precision)
        g.fillRect(x - Config::Layout::Glow::cutMarkerWidthThin * 0.5f,
                   (float)bounds.getY() + boxHeight + pillarHeight, 
                   Config::Layout::Glow::cutMarkerWidthThin,
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
