

#include "UI/Views/ZoomView.h"
#include "UI/ControlPanel.h"
#include "UI/Views/PlaybackCursorGlow.h"
#include "Utils/Config.h"

ZoomView::ZoomView(ControlPanel &ownerIn) : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);

    setOpaque(false);
}

ZoomView::~ZoomView() = default;

void ZoomView::paint(juce::Graphics& g) {
    if (state.audioLength <= 0.0) return;
    
    drawMouseCursor(g);
    drawZoomPopup(g);
    drawHud(g);
}

void ZoomView::drawMouseCursor(juce::Graphics& g) {
    if (state.mouseX == -1) return;

    const auto waveformBounds = getLocalBounds();
    const int localMouseX = state.mouseX;
    const int localMouseY = state.mouseY;

    const float centerY = (float)waveformBounds.getCentreY();
    const float amplitudeY = state.amplitudeY;
    const float bottomAmplitudeY = state.bottomAmplitudeY;

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
               (int)amplitudeY - Config::Layout::Text::mouseCursorSize, 200,
               Config::Layout::Text::mouseCursorSize, juce::Justification::left, true);
    g.drawText(state.negAmplitudeText, localMouseX + Config::Layout::Glow::mouseTextOffset,
               (int)bottomAmplitudeY, 200, Config::Layout::Text::mouseCursorSize,
               juce::Justification::left, true);

    const juce::String timeText = state.mouseTimeText;
    g.drawText(timeText, localMouseX + Config::Layout::Glow::mouseTextOffset,
               localMouseY + Config::Layout::Glow::mouseTextOffset, 200,
               Config::Layout::Text::mouseCursorSize, juce::Justification::left, true);

    g.setColour(state.cursorLineColor);
    g.drawHorizontalLine(localMouseY, (float)waveformBounds.getX(),
                         (float)waveformBounds.getRight());
}

void ZoomView::drawZoomPopup(juce::Graphics& g) {
    if (!state.isZooming || state.thumbnail == nullptr) return;

    const juce::Rectangle<int> popupBounds = state.popupBounds;
    const double startTime = state.startTime;
    const double endTime = state.endTime;

    if (isCacheDirty || waveformCache.isNull() || 
        waveformCache.getWidth() != popupBounds.getWidth() || 
        waveformCache.getHeight() != popupBounds.getHeight()) 
    {
        waveformCache = juce::Image(juce::Image::ARGB, popupBounds.getWidth(), popupBounds.getHeight(), true);
        juce::Graphics imgG(waveformCache);

        imgG.setColour(Config::Colors::solidBlack);
        imgG.fillAll();

        juce::ColourGradient gradient(Config::Colors::waveformPeak, 0, 0,
                                      Config::Colors::waveformPeak, 0, (float)popupBounds.getHeight(), false);
        gradient.addColour(0.5, Config::Colors::waveformCore);
        imgG.setGradientFill(gradient);

        auto localBounds = waveformCache.getBounds();
        if (state.channelMode == AppEnums::ChannelViewMode::Mono || state.numChannels == 1) {
            state.thumbnail->drawChannel(imgG, localBounds, startTime, endTime, 0, 1.0f);
            imgG.setColour(Config::Colors::zoomPopupZeroLine);
            imgG.drawHorizontalLine(localBounds.getCentreY(), 0.0f, (float)localBounds.getWidth());
        } else {
            auto topBounds = localBounds.withHeight(localBounds.getHeight() / 2);
            auto bottomBounds = localBounds.withTop(topBounds.getBottom()).withHeight(localBounds.getHeight() / 2);
            state.thumbnail->drawChannel(imgG, topBounds, startTime, endTime, 0, 1.0f);
            state.thumbnail->drawChannel(imgG, bottomBounds, startTime, endTime, 1, 1.0f);
            imgG.setColour(Config::Colors::zoomPopupZeroLine);
            imgG.drawHorizontalLine(topBounds.getCentreY(), 0.0f, (float)topBounds.getWidth());
            imgG.drawHorizontalLine(bottomBounds.getCentreY(), 0.0f, (float)bottomBounds.getWidth());
        }
        isCacheDirty = false;
    }

    g.drawImageAt(waveformCache, popupBounds.getX(), popupBounds.getY());

    auto drawShadow = [&](float x1, float x2, juce::Colour color) {
        if (x1 >= x2) return;
        g.setColour(color);
        g.fillRect(x1, (float)popupBounds.getY(), x2 - x1, (float)popupBounds.getHeight());
    };

    const float inX = state.cutInPixelX;
    const float outX = state.cutOutPixelX;
    const float actualInX = juce::jmin(inX, outX);
    const float actualOutX = juce::jmax(inX, outX);

    drawShadow((float)popupBounds.getX(), actualInX, Config::Colors::solidBlack);
    drawShadow(actualOutX, (float)popupBounds.getRight(), Config::Colors::solidBlack);

    auto drawFineLine = [&](float x, juce::Colour color, float thickness) {
        if (x >= (float)popupBounds.getX() && x <= (float)popupBounds.getRight()) {
            g.setColour(color);
            g.drawLine(x, (float)popupBounds.getY(), x, (float)popupBounds.getBottom(),
                       thickness);
        }
    };

    drawFineLine(inX, Config::Colors::cutLine,
                 Config::Layout::connectorLineWidth);
    drawFineLine(outX, Config::Colors::cutLine,
                 Config::Layout::connectorLineWidth);
    drawFineLine(state.currentPositionPixelX,
                 Config::Colors::playbackCursor, 
                 Config::Layout::buttonOutlineThickness);

    if (state.isDraggingCutIn || state.isDraggingCutOut) {
        const juce::Colour trackingColor = Config::Colors::zoomPopupTrackingLine;
        drawFineLine(state.isDraggingCutIn ? inX : outX, trackingColor,
                     Config::Layout::connectorLineWidth);
    } else {
        drawFineLine(state.currentPositionPixelX,
                     Config::Colors::zoomPopupPlaybackLine,
                     Config::Layout::connectorLineWidth);
    }

    g.setColour(Config::Colors::zoomPopupBorder);
    g.drawRect(popupBounds.toFloat(), Config::Layout::Zoom::borderThickness);
}

void ZoomView::drawHud(juce::Graphics& g) {
    if (state.hudLines.empty()) return;

    const int hudX = getWidth() - 200 - Config::Layout::Zoom::hudPadding;
    const int hudY = Config::Layout::Zoom::hudPadding;
    const int hudWidth = 200;
    const int hudHeight = (int)state.hudLines.size() * Config::Layout::Zoom::hudLineSpacing + Config::Layout::Zoom::hudPadding;

    g.setColour(Config::Colors::ZoomHud::background);
    g.fillRoundedRectangle((float)hudX, (float)hudY, (float)hudWidth, (float)hudHeight, 4.0f);

    g.setFont((float)Config::Layout::Zoom::hudFontSize);
    for (size_t i = 0; i < state.hudLines.size(); ++i) {
        g.setColour(state.hudLines[i].isActive ? Config::Colors::ZoomHud::textActive : Config::Colors::ZoomHud::textInactive);
        g.drawText(state.hudLines[i].text, hudX + Config::Layout::Zoom::hudPadding,
                   hudY + Config::Layout::Zoom::hudPadding + (int)i * Config::Layout::Zoom::hudLineSpacing,
                   hudWidth - 2 * Config::Layout::Zoom::hudPadding, Config::Layout::Zoom::hudLineSpacing,
                   juce::Justification::left, true);
    }
}
