

#include "UI/Views/CutLayerView.h"

#include "Core/SessionState.h"
#include "Core/WaveformManager.h"
#include "UI/ControlPanel.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "Utils/CoordinateMapper.h"
#include "Workers/SilenceDetector.h"

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
    const float audioLength = (float)state.audioLength;
    if (audioLength <= 0.0f)
        return;

    auto drawThresholdVisualisation = [&](double cutPos, float threshold) {
        const float normalisedThreshold = threshold;
        const float centerY = (float)bounds.getCentreY();
        const float halfHeight = (float)bounds.getHeight() / 2.0f;

        float topThresholdY = centerY - (normalisedThreshold * halfHeight);
        float bottomThresholdY = centerY + (normalisedThreshold * halfHeight);

        topThresholdY =
            juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), topThresholdY);
        bottomThresholdY =
            juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), bottomThresholdY);

        const float xPos = (float)bounds.getX() +
                           CoordinateMapper::secondsToPixels(cutPos, (float)bounds.getWidth(),
                                                             (double)audioLength);
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
            g.fillRect(lineStartX, topThresholdY - 2.5f, currentLineWidth, 5.0f);
            g.fillRect(lineStartX, bottomThresholdY - 2.5f, currentLineWidth, 5.0f);
        }

        g.setColour(Config::Colors::thresholdLine);
        g.drawHorizontalLine((int)topThresholdY, lineStartX, lineEndX);
        g.drawHorizontalLine((int)bottomThresholdY, lineStartX, lineEndX);
    };

    const double cutIn = state.cutInSeconds;
    const double cutOut = state.cutOutSeconds;
    drawThresholdVisualisation(cutIn, state.inThreshold);
    drawThresholdVisualisation(cutOut, state.outThreshold);

    const double actualIn = juce::jmin(cutIn, cutOut);
    const double actualOut = juce::jmax(cutIn, cutOut);

    const float inX = juce::jlimit(
        (float)bounds.getX(), (float)bounds.getRight(),
        (float)bounds.getX() + CoordinateMapper::secondsToPixels(actualIn, (float)bounds.getWidth(),
                                                                 (double)audioLength));
    const float outX = juce::jlimit(
        (float)bounds.getX(), (float)bounds.getRight(),
        (float)bounds.getX() + CoordinateMapper::secondsToPixels(
                                   actualOut, (float)bounds.getWidth(), (double)audioLength));

    const float fadeLength = bounds.getWidth() * Config::Layout::Waveform::cutRegionFadeProportion;
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;

    const juce::Rectangle<float> leftRegion((float)bounds.getX(), (float)bounds.getY(),
                                            juce::jmax(0.0f, inX - (float)bounds.getX()),
                                            (float)bounds.getHeight());
    if (leftRegion.getWidth() > 0.0f) {
        const float actualFade = juce::jmin(fadeLength, leftRegion.getWidth());

        juce::Rectangle<float> solidBlackLeft =
            leftRegion.withWidth(juce::jmax(0.0f, leftRegion.getWidth() - actualFade));
        g.setColour(juce::Colours::black);
        g.fillRect(solidBlackLeft);

        juce::Rectangle<float> fadeAreaLeft(inX - actualFade, (float)bounds.getY(), actualFade,
                                            (float)bounds.getHeight());
        juce::ColourGradient leftFadeGradient(Config::Colors::cutRegion, inX,
                                              leftRegion.getCentreY(), juce::Colours::black,
                                              inX - actualFade, leftRegion.getCentreY(), false);
        g.setGradientFill(leftFadeGradient);
        g.fillRect(fadeAreaLeft);
    }

    const juce::Rectangle<float> rightRegion(outX, (float)bounds.getY(),
                                             juce::jmax(0.0f, (float)bounds.getRight() - outX),
                                             (float)bounds.getHeight());
    if (rightRegion.getWidth() > 0.0f) {
        const float actualFade = juce::jmin(fadeLength, rightRegion.getWidth());

        float solidBlackStart = outX + actualFade;
        juce::Rectangle<float> solidBlackRight(
            solidBlackStart, (float)bounds.getY(),
            juce::jmax(0.0f, (float)bounds.getRight() - solidBlackStart),
            (float)bounds.getHeight());
        g.setColour(juce::Colours::black);
        g.fillRect(solidBlackRight);

        juce::Rectangle<float> fadeAreaRight(outX, (float)bounds.getY(), actualFade,
                                             (float)bounds.getHeight());
        juce::ColourGradient rightFadeGradient(Config::Colors::cutRegion, outX,
                                               rightRegion.getCentreY(), juce::Colours::black,
                                               outX + actualFade, rightRegion.getCentreY(), false);
        g.setGradientFill(rightFadeGradient);
        g.fillRect(fadeAreaRight);
    }

    auto drawCutMarker = [&](float x, MarkerMouseHandler::CutMarkerHandle handleType) {
        juce::Colour markerColor = Config::Colors::cutLine;

        if (handleType == MarkerMouseHandler::CutMarkerHandle::In && state.isAutoIn)
            markerColor = Config::Colors::cutMarkerAuto;
        else if (handleType == MarkerMouseHandler::CutMarkerHandle::Out && state.isAutoOut)
            markerColor = Config::Colors::cutMarkerAuto;

        float thickness = Config::Layout::Glow::cutBoxOutlineThickness;
        bool shouldPulse = false;

        const bool isDragged = state.draggedHandle == handleType || state.draggedHandle == MarkerMouseHandler::CutMarkerHandle::Full;
        const bool isHovered = state.hoveredHandle == handleType || state.hoveredHandle == MarkerMouseHandler::CutMarkerHandle::Full;

        if (isDragged) {
            markerColor = Config::Colors::cutMarkerDrag;
            thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
            shouldPulse = true;
        } else if (isHovered) {
            markerColor = Config::Colors::cutMarkerHover;
            thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
            shouldPulse = true;
        }

        // Draw Glow if active
        if (shouldPulse && state.showEyeCandy) {
            const float pulse = state.glowAlpha;
            const juce::Colour glowColor = Config::Colors::cutLine.withAlpha(
                Config::Colors::cutLine.getFloatAlpha() * (0.2f + 0.8f * pulse));
            g.setColour(glowColor);
            g.fillRect(x - (Config::Layout::Glow::cutLineGlowThickness *
                                Config::Layout::Glow::offsetFactor -
                            0.5f),
                       (float)bounds.getY() + boxHeight, Config::Layout::Glow::cutLineGlowThickness,
                       (float)bounds.getHeight() - (2.0f * boxHeight));
        } else {
            g.setColour(Config::Colors::cutLine.withAlpha(0.3f));
            g.fillRect(x - 0.5f, (float)bounds.getY() + boxHeight, 1.0f,
                       (float)bounds.getHeight() - (2.0f * boxHeight));
        }

        const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
        const float halfBoxWidth = boxWidth / 2.0f;

        g.setColour(markerColor);
        g.drawRect(x - halfBoxWidth, (float)bounds.getY(), boxWidth, boxHeight, thickness);
        g.drawRect(x - halfBoxWidth, (float)bounds.getBottom() - boxHeight, boxWidth, boxHeight,
                   thickness);

        g.setColour(markerColor);
        g.fillRect(x - Config::Layout::Glow::cutMarkerWidthThin /
                           Config::Layout::Glow::cutMarkerCenterDivisor,
                   (float)bounds.getY() + boxHeight, Config::Layout::Glow::cutMarkerWidthThin,
                   (float)bounds.getHeight() - (2.0f * boxHeight));
    };

    drawCutMarker(inX, MarkerMouseHandler::CutMarkerHandle::In);
    drawCutMarker(outX, MarkerMouseHandler::CutMarkerHandle::Out);

    juce::Colour hollowColor = Config::Colors::cutLine;
    float hollowThickness = Config::Layout::Glow::cutBoxOutlineThickness;
    bool regionActive = state.draggedHandle == MarkerMouseHandler::CutMarkerHandle::Full ||
                       state.hoveredHandle == MarkerMouseHandler::CutMarkerHandle::Full;

    if (state.draggedHandle == MarkerMouseHandler::CutMarkerHandle::Full) {
        hollowColor = Config::Colors::cutMarkerDrag;
        hollowThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    } else if (state.hoveredHandle == MarkerMouseHandler::CutMarkerHandle::Full) {
        hollowColor = Config::Colors::cutMarkerHover;
        hollowThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    }

    if (regionActive && state.showEyeCandy)
        g.setColour(hollowColor.withAlpha(0.5f + 0.5f * state.glowAlpha));
    else
        g.setColour(hollowColor.withAlpha(0.4f));

    const float halfBoxWidth = Config::Layout::Glow::cutMarkerBoxWidth / 2.0f;
    const float startX = inX + halfBoxWidth;
    const float endX = outX - halfBoxWidth;

    if (startX < endX) {
        g.drawLine(startX, (float)bounds.getY(), endX, (float)bounds.getY(), hollowThickness);
        g.drawLine(startX, (float)bounds.getY() + boxHeight, endX, (float)bounds.getY() + boxHeight,
                   hollowThickness);
        g.drawLine(startX, (float)bounds.getBottom() - 1.0f, endX, (float)bounds.getBottom() - 1.0f,
                   hollowThickness);
        g.drawLine(startX, (float)bounds.getBottom() - boxHeight, endX,
                   (float)bounds.getBottom() - boxHeight, hollowThickness);
    }
}
