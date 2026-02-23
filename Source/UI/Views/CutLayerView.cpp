

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
    setBufferedToImage(true);
}

CutLayerView::~CutLayerView() = default;

void CutLayerView::updateState(const CutLayerState& newState) {
    state = newState;
    repaint();
}

void CutLayerView::paint(juce::Graphics &g) {
    if (!state.markersVisible)
        return;

    const auto bounds = getLocalBounds();
    if (state.audioLength <= 0.0f)
        return;

    auto drawThresholdVisualisation = [&](float xPos, float topThresholdY, float bottomThresholdY) {
        const float halfThresholdLineWidth = Config::Animation::thresholdLineWidth / 2.0f;
        float lineStartX = xPos - halfThresholdLineWidth;
        float lineEndX = xPos + halfThresholdLineWidth;

        lineStartX = juce::jmax(lineStartX, (float)bounds.getX());
        lineEndX = juce::jmin(lineEndX, (float)bounds.getRight());
        const float currentLineWidth = lineEndX - lineStartX;

        g.setColour(Config::Colors::thresholdRegion);
        g.fillRect(lineStartX, topThresholdY, currentLineWidth, bottomThresholdY - topThresholdY);

        if (state.showEyeCandy) {
            const float pulse = state.glowAlpha;
            const juce::Colour glowColor =
                Config::Colors::thresholdLine.withAlpha(0.2f + 0.6f * pulse);
            g.setColour(glowColor);

            // Draw a wider rectangle behind the line for a glow effect
            g.fillRect(lineStartX, topThresholdY - Config::Layout::outlineThicknessMedium,
                       currentLineWidth, Config::Layout::buttonCornerRadius);
            g.fillRect(lineStartX, bottomThresholdY - Config::Layout::outlineThicknessMedium,
                       currentLineWidth, Config::Layout::buttonCornerRadius);
        }

        g.setColour(Config::Colors::thresholdLine);
        g.drawHorizontalLine((int)topThresholdY, lineStartX, lineEndX);
        g.drawHorizontalLine((int)bottomThresholdY, lineStartX, lineEndX);
    };

    drawThresholdVisualisation(state.inPixelX, state.inThresholdYTop, state.inThresholdYBottom);
    drawThresholdVisualisation(state.outPixelX, state.outThresholdYTop, state.outThresholdYBottom);

    const float actualInX = state.actualInX;
    const float actualOutX = state.actualOutX;

    const float fadeLength = state.fadeWidthPixels;
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;

    const juce::Rectangle<float> leftRegion((float)bounds.getX(), (float)bounds.getY(),
                                            juce::jmax(0.0f, actualInX - (float)bounds.getX()),
                                            (float)bounds.getHeight());
    if (leftRegion.getWidth() > 0.0f) {
        const float actualFade = juce::jmin(fadeLength, leftRegion.getWidth());

        juce::Rectangle<float> solidBlackLeft =
            leftRegion.withWidth(juce::jmax(0.0f, leftRegion.getWidth() - actualFade));
        g.setColour(Config::Colors::solidBlack);
        g.fillRect(solidBlackLeft);

        juce::Rectangle<float> fadeAreaLeft(actualInX - actualFade, (float)bounds.getY(), actualFade,
                                            (float)bounds.getHeight());
        juce::ColourGradient leftFadeGradient(Config::Colors::cutRegion, actualInX,
                                              leftRegion.getCentreY(), Config::Colors::solidBlack,
                                              actualInX - actualFade, leftRegion.getCentreY(), false);
        g.setGradientFill(leftFadeGradient);
        g.fillRect(fadeAreaLeft);
    }

    const juce::Rectangle<float> rightRegion(actualOutX, (float)bounds.getY(),
                                             juce::jmax(0.0f, (float)bounds.getRight() - actualOutX),
                                             (float)bounds.getHeight());
    if (rightRegion.getWidth() > 0.0f) {
        const float actualFade = juce::jmin(fadeLength, rightRegion.getWidth());

        float solidBlackStart = actualOutX + actualFade;
        juce::Rectangle<float> solidBlackRight(
            solidBlackStart, (float)bounds.getY(),
            juce::jmax(0.0f, (float)bounds.getRight() - solidBlackStart),
            (float)bounds.getHeight());
        g.setColour(Config::Colors::solidBlack);
        g.fillRect(solidBlackRight);

        juce::Rectangle<float> fadeAreaRight(actualOutX, (float)bounds.getY(), actualFade,
                                             (float)bounds.getHeight());
        juce::ColourGradient rightFadeGradient(Config::Colors::cutRegion, actualOutX,
                                               rightRegion.getCentreY(), Config::Colors::solidBlack,
                                               actualOutX + actualFade, rightRegion.getCentreY(), false);
        g.setGradientFill(rightFadeGradient);
        g.fillRect(fadeAreaRight);
    }

    auto drawCutMarker = [&](float x, juce::Colour markerColor, float thickness, bool shouldPulse) {
        const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
        const float halfBoxWidth = boxWidth / 2.0f;
        const float pillarHeight = (float)bounds.getHeight() * 0.15f;

        // Draw Glow if active
        if (shouldPulse && state.showEyeCandy) {
            const float pulse = state.glowAlpha;
            const juce::Colour glowColor = Config::Colors::cutLine.withAlpha(
                Config::Colors::cutLine.getFloatAlpha() * (0.2f + 0.8f * pulse));
            g.setColour(glowColor);
            g.fillRect(x - (Config::Layout::Glow::cutLineGlowThickness *
                                Config::Layout::Glow::offsetFactor -
                            Config::Layout::buttonOutlineThickness * 0.5f),
                       (float)bounds.getY() + boxHeight, Config::Layout::Glow::cutLineGlowThickness,
                       (float)bounds.getHeight() - (2.0f * boxHeight));
        } else {
            g.setColour(Config::Colors::cutLine.withAlpha(0.3f));
            g.fillRect(x - Config::Layout::buttonOutlineThickness * 0.5f, (float)bounds.getY() + boxHeight,
                       Config::Layout::buttonOutlineThickness,
                       (float)bounds.getHeight() - (2.0f * boxHeight));
        }

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

    if (state.regionShouldPulse && state.showEyeCandy)
        g.setColour(state.regionOutlineColor.withAlpha(0.5f + 0.5f * state.glowAlpha));
    else
        g.setColour(state.regionOutlineColor.withAlpha(0.4f));

    const float halfBoxWidth = Config::Layout::Glow::cutMarkerBoxWidth / 2.0f;
    const float startX = actualInX + halfBoxWidth;
    const float endX = actualOutX - halfBoxWidth;

    if (startX < endX) {
        const float hThickness = state.regionOutlineThickness;
        // Top edge bar
        g.fillRect(startX, (float)bounds.getY(), endX - startX, hThickness);
        // Box bottom horizontal bar
        g.fillRect(startX, (float)bounds.getY() + boxHeight - hThickness, endX - startX, hThickness);
        
        // Bottom edge bar
        g.fillRect(startX, (float)bounds.getBottom() - hThickness, endX - startX, hThickness);
        // Box top horizontal bar
        g.fillRect(startX, (float)bounds.getBottom() - boxHeight, endX - startX, hThickness);
    }
}
