#include "UI/Views/CutFadeView.h"

#include "Utils/Config.h"

CutFadeView::CutFadeView() {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

CutFadeView::~CutFadeView() = default;

void CutFadeView::updateState(const CutLayerState& newState) {
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

void CutFadeView::paint(juce::Graphics &g) {
    if (!state.markersVisible)
        return;

    if (state.audioLength <= 0.0f)
        return;

    g.setColour(Config::Colors::solidBlack);
    const float actualInX = state.actualInX;
    const float actualOutX = state.actualOutX;
    const auto bounds = getLocalBounds().toFloat();

    if (actualInX > bounds.getX()) {
        g.fillRect(bounds.getX(), bounds.getY(), actualInX - bounds.getX(), bounds.getHeight());
    }

    if (actualOutX < bounds.getRight()) {
        g.fillRect(actualOutX, bounds.getY(), bounds.getRight() - actualOutX, bounds.getHeight());
    }
}
