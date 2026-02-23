

#include "UI/Views/ZoomView.h"
#include "UI/ControlPanel.h"
#include "UI/Views/PlaybackCursorGlow.h"
#include "Utils/Config.h"

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

        const float centerY = (float)waveformBounds.getCentreY();
        const float amplitudeY = state.amplitudeY;
        const float bottomAmplitudeY = state.bottomAmplitudeY;

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
        g.drawText(state.amplitudeText, localMouseX + Config::Layout::Glow::mouseTextOffset,
                   (int)amplitudeY - Config::Layout::Text::mouseCursorSize, 100,
                   Config::Layout::Text::mouseCursorSize, juce::Justification::left, true);
        g.drawText(state.negAmplitudeText, localMouseX + Config::Layout::Glow::mouseTextOffset,
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

        g.setColour(Config::Colors::solidBlack);
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

        auto drawShadow = [&](float x1, float x2, juce::Colour color) {
            if (x1 >= x2) return;
            g.setColour(color);
            g.fillRect(x1, (float)popupBounds.getY(), x2 - x1, (float)popupBounds.getHeight());
        };

        const float inX = state.cutInPixelX;
        const float outX = state.cutOutPixelX;
        const float actualInX = juce::jmin(inX, outX);
        const float actualOutX = juce::jmax(inX, outX);

        drawShadow((float)popupBounds.getX(), actualInX, Config::Colors::solidBlack.withAlpha(0.5f));
        drawShadow(actualOutX, (float)popupBounds.getRight(), Config::Colors::solidBlack.withAlpha(0.5f));

        const float pulse = state.eyeCandyPulse;

        auto drawFineLine = [&](float x, juce::Colour color, float thickness) {
            if (x >= (float)popupBounds.getX() && x <= (float)popupBounds.getRight()) {
                g.setColour(color);
                g.drawLine(x, (float)popupBounds.getY(), x, (float)popupBounds.getBottom(),
                           thickness);
            }
        };

        drawFineLine(inX, Config::Colors::cutLine.withAlpha(0.7f + 0.3f * pulse),
                     Config::Layout::connectorLineWidth);
        drawFineLine(outX, Config::Colors::cutLine.withAlpha(0.7f + 0.3f * pulse),
                     Config::Layout::connectorLineWidth);
        drawFineLine(state.currentPositionPixelX,
                     Config::Colors::playbackCursor.withAlpha(0.6f + 0.4f * pulse), 
                     Config::Layout::buttonOutlineThickness);

        if (state.isDraggingCutIn || state.isDraggingCutOut) {
            const juce::Colour trackingColor =
                Config::Colors::zoomPopupTrackingLine.withAlpha(0.8f + 0.2f * pulse);
            drawFineLine(state.isDraggingCutIn ? inX : outX, trackingColor,
                         Config::Layout::connectorLineWidth + 0.5f * pulse);
        } else {
            drawFineLine(state.currentPositionPixelX,
                         Config::Colors::zoomPopupPlaybackLine.withAlpha(0.7f + 0.3f * pulse),
                         Config::Layout::connectorLineWidth);
        }

        g.setColour(Config::Colors::zoomPopupBorder);
        g.drawRect(popupBounds.toFloat(), Config::Layout::Zoom::borderThickness);
    }
}
