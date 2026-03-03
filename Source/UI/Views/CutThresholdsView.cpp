#include "UI/Views/CutThresholdsView.h"

#include "Utils/Config.h"

CutThresholdsView::CutThresholdsView() {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

CutThresholdsView::~CutThresholdsView() = default;

void CutThresholdsView::updateState(const CutLayerState& newState) {
    const bool thresholdsMoved =
        state.inThresholdYTop != newState.inThresholdYTop ||
        state.inThresholdYBottom != newState.inThresholdYBottom ||
        state.outThresholdYTop != newState.outThresholdYTop ||
        state.outThresholdYBottom != newState.outThresholdYBottom;

    const bool visibilityChanged =
        state.markersVisible != newState.markersVisible ||
        state.audioLength != newState.audioLength;

    const bool colorChanged =
        state.inThresholdColor != newState.inThresholdColor ||
        state.outThresholdColor != newState.outThresholdColor;

    if (thresholdsMoved || visibilityChanged || colorChanged)
        repaint();

    state = newState;
}

void CutThresholdsView::paint(juce::Graphics &g) {
    if (!state.markersVisible)
        return;

    if (state.audioLength <= 0.0f)
        return;

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
