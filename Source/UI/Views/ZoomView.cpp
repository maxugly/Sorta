

#include "UI/Views/ZoomView.h"
#include "UI/ControlPanel.h"
#include "UI/Views/PlaybackCursorGlow.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"

ZoomView::ZoomView(ControlPanel &ownerIn) : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);

    setOpaque(false);
}

ZoomView::~ZoomView() = default;

void ZoomView::paint(juce::Graphics &g) {
    if (state.audioLength <= 0.0)
        return;

    const auto waveformBounds = getLocalBounds();

    if (state.mouseX != -1) {
        const int localMouseX = state.mouseX;
        const int localMouseY = state.mouseY;

        juce::Colour currentLineColor = Config::Colors::mouseCursorLine;
        juce::Colour currentHighlightColor = Config::Colors::mouseCursorHighlight;
        juce::Colour currentGlowColor = Config::Colors::mouseAmplitudeGlow;
        float currentGlowThickness = Config::Layout::Glow::thickness;

        if (state.placementMode == AppEnums::PlacementMode::CutIn ||
            state.placementMode == AppEnums::PlacementMode::CutOut) {
            currentLineColor = Config::Colors::mousePlacementMode;
            currentHighlightColor = Config::Colors::mousePlacementMode.withAlpha(0.4f);
            currentGlowColor = Config::Colors::placementModeGlow;
            currentGlowThickness = Config::Layout::Glow::placementModeGlowThickness;

            g.setColour(currentGlowColor.withAlpha(Config::Layout::Glow::mouseAlpha));
            g.fillRect(localMouseX -
                           (int)(currentGlowThickness * Config::Layout::Glow::offsetFactor) - 1,
                       waveformBounds.getY(),
                       (int)currentGlowThickness + Config::Layout::Glow::mousePadding,
                       waveformBounds.getHeight());
            g.fillRect(waveformBounds.getX(),
                       localMouseY -
                           (int)(currentGlowThickness * Config::Layout::Glow::offsetFactor) - 1,
                       waveformBounds.getWidth(),
                       (int)currentGlowThickness + Config::Layout::Glow::mousePadding);
        } else if (state.isZKeyDown) {
            currentLineColor = Config::Colors::mousePlacementMode;
            currentHighlightColor = Config::Colors::mousePlacementMode.withAlpha(0.4f);
        }

        g.setColour(currentHighlightColor);
        g.fillRect(localMouseX - Config::Layout::Glow::mouseHighlightOffset, waveformBounds.getY(),
                   Config::Layout::Glow::mouseHighlightSize, waveformBounds.getHeight());
        g.fillRect(waveformBounds.getX(), localMouseY - Config::Layout::Glow::mouseHighlightOffset,
                   waveformBounds.getWidth(), Config::Layout::Glow::mouseHighlightSize);

        const float amplitude = state.amplitude;
        const float centerY = (float)waveformBounds.getCentreY();
        const float amplitudeY = centerY - (amplitude * waveformBounds.getHeight() *
                                            Config::Layout::Waveform::heightScale);
        const float bottomAmplitudeY = centerY + (amplitude * waveformBounds.getHeight() *
                                                  Config::Layout::Waveform::heightScale);

        juce::ColourGradient amplitudeGlowGradient(
            currentGlowColor.withAlpha(0.0f), (float)localMouseX, amplitudeY,
            currentGlowColor.withAlpha(Config::Layout::Glow::mouseAmplitudeAlpha),
            (float)localMouseX, centerY, true);
        g.setGradientFill(amplitudeGlowGradient);
        g.fillRect(juce::Rectangle<float>(
            (float)localMouseX - Config::Layout::Glow::mouseAmplitudeGlowThickness *
                                     Config::Layout::Glow::offsetFactor,
            amplitudeY, Config::Layout::Glow::mouseAmplitudeGlowThickness, centerY - amplitudeY));
        g.fillRect(juce::Rectangle<float>(
            (float)localMouseX - Config::Layout::Glow::mouseAmplitudeGlowThickness *
                                     Config::Layout::Glow::offsetFactor,
            centerY, Config::Layout::Glow::mouseAmplitudeGlowThickness, centerY - amplitudeY));

        g.setColour(Config::Colors::mouseAmplitudeLine);
        g.drawVerticalLine(localMouseX, amplitudeY, bottomAmplitudeY);

        const float halfLineLength =
            Config::Animation::mouseAmplitudeLineLength * Config::Layout::Glow::offsetFactor;
        const float leftExtent = (float)localMouseX - halfLineLength;
        const float rightExtent = (float)localMouseX + halfLineLength;
        g.drawHorizontalLine(juce::roundToInt(amplitudeY), leftExtent, rightExtent);
        g.drawHorizontalLine(juce::roundToInt(bottomAmplitudeY), leftExtent, rightExtent);

        g.setColour(Config::Colors::playbackText);
        g.setFont(Config::Layout::Text::mouseCursorSize);
        g.drawText(juce::String(amplitude, 2), localMouseX + Config::Layout::Glow::mouseTextOffset,
                   (int)amplitudeY - Config::Layout::Text::mouseCursorSize, 100,
                   Config::Layout::Text::mouseCursorSize, juce::Justification::left, true);
        g.drawText(juce::String(-amplitude, 2), localMouseX + Config::Layout::Glow::mouseTextOffset,
                   (int)bottomAmplitudeY, 100, Config::Layout::Text::mouseCursorSize,
                   juce::Justification::left, true);

        const juce::String timeText = state.mouseTimeText;
        g.drawText(timeText, localMouseX + Config::Layout::Glow::mouseTextOffset,
                   localMouseY + Config::Layout::Glow::mouseTextOffset, 100,
                   Config::Layout::Text::mouseCursorSize, juce::Justification::left, true);

        const juce::Colour glowColor = state.shouldShowEyeCandy
                                           ? currentLineColor
                                           : currentLineColor.withAlpha(0.0f);
        PlaybackCursorGlow::renderGlow(g, localMouseX, waveformBounds.getY(),
                                       waveformBounds.getBottom(), glowColor);
        g.setColour(currentLineColor);
        g.drawHorizontalLine(localMouseY, (float)waveformBounds.getX(),
                             (float)waveformBounds.getRight());
    }

    if (state.isZooming && state.thumbnail != nullptr) {
        const juce::Rectangle<int> popupBounds = state.popupBounds;
        const double startTime = state.startTime;
        const double endTime = state.endTime;
        const double timeRange = endTime - startTime;

        g.setColour(juce::Colours::black);
        g.fillRect(popupBounds);

        g.setColour(Config::Colors::waveform);
        
        if (state.channelMode == AppEnums::ChannelViewMode::Mono || state.numChannels == 1) {
            state.thumbnail->drawChannel(g, popupBounds, startTime,
                                                                        endTime, 0, 1.0f);
            g.setColour(Config::Colors::zoomPopupZeroLine);
            g.drawHorizontalLine(popupBounds.getCentreY(), (float)popupBounds.getX(),
                                 (float)popupBounds.getRight());
        } else {
            auto topBounds = popupBounds.withHeight(popupBounds.getHeight() / 2);
            auto bottomBounds =
                popupBounds.withTop(topBounds.getBottom()).withHeight(popupBounds.getHeight() / 2);

            state.thumbnail->drawChannel(g, topBounds, startTime,
                                                                        endTime, 0, 1.0f);
            state.thumbnail->drawChannel(g, bottomBounds, startTime,
                                                                        endTime, 1, 1.0f);

            g.setColour(Config::Colors::zoomPopupZeroLine);
            g.drawHorizontalLine(topBounds.getCentreY(), (float)topBounds.getX(),
                                 (float)topBounds.getRight());
            g.drawHorizontalLine(bottomBounds.getCentreY(), (float)bottomBounds.getX(),
                                 (float)bottomBounds.getRight());
        }

        auto drawShadow = [&](double startT, double endT, juce::Colour color) {
            if (endT <= startTime || startT >= endTime)
                return;
            double vStart = juce::jmax(startT, startTime);
            double vEnd = juce::jmin(endT, endTime);
            float x1 = (float)popupBounds.getX() +
                       CoordinateMapper::secondsToPixels(vStart - startTime,
                                                         (float)popupBounds.getWidth(), timeRange);
            float x2 = (float)popupBounds.getX() +
                       CoordinateMapper::secondsToPixels(vEnd - startTime,
                                                         (float)popupBounds.getWidth(), timeRange);
            g.setColour(color);
            g.fillRect(x1, (float)popupBounds.getY(), x2 - x1, (float)popupBounds.getHeight());
        };

        const double cutIn = state.cutIn;
        const double cutOut = state.cutOut;

        drawShadow(startTime, cutIn, juce::Colours::black.withAlpha(0.5f));
        drawShadow(cutOut, endTime, juce::Colours::black.withAlpha(0.5f));

        if (startTime < 0.0)
            drawShadow(startTime, 0.0, juce::Colours::black);
        if (endTime > state.audioLength)
            drawShadow(state.audioLength, endTime, juce::Colours::black);

        const float pulse = state.eyeCandyPulse;

        auto drawFineLine = [&](double time, juce::Colour color, float thickness) {
            if (time >= startTime && time <= endTime) {
                float x = (float)popupBounds.getX() +
                          CoordinateMapper::secondsToPixels(
                              time - startTime, (float)popupBounds.getWidth(), timeRange);
                g.setColour(color);
                g.drawLine(x, (float)popupBounds.getY(), x, (float)popupBounds.getBottom(),
                           thickness);
            }
        };

        drawFineLine(cutIn, Config::Colors::cutLine.withAlpha(0.7f + 0.3f * pulse), 2.0f);
        drawFineLine(cutOut, Config::Colors::cutLine.withAlpha(0.7f + 0.3f * pulse), 2.0f);
        drawFineLine(state.currentPosition,
                     Config::Colors::playbackCursor.withAlpha(0.6f + 0.4f * pulse), 1.0f);

        if (state.isDraggingCutIn || state.isDraggingCutOut) {
            const juce::Colour trackingColor =
                Config::Colors::zoomPopupTrackingLine.withAlpha(0.8f + 0.2f * pulse);
            drawFineLine(state.isDraggingCutIn ? cutIn : cutOut, trackingColor, 2.0f + 0.5f * pulse);
        } else {
            drawFineLine(state.currentPosition,
                         Config::Colors::zoomPopupPlaybackLine.withAlpha(0.7f + 0.3f * pulse),
                         2.0f);
        }

        g.setColour(Config::Colors::zoomPopupBorder);
        g.drawRect(popupBounds.toFloat(), Config::Layout::Zoom::borderThickness);
    }
}
