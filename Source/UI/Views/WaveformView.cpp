#include "UI/Views/WaveformView.h"
#include "Core/WaveformManager.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"

WaveformView::WaveformView(WaveformManager &waveformManagerIn)
    : waveformManager(waveformManagerIn) {
    waveformManager.addChangeListener(this);
    setInterceptsMouseClicks(false, false);
    setOpaque(true);
}

WaveformView::~WaveformView() {
    waveformManager.removeChangeListener(this);
}

void WaveformView::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &waveformManager.getThumbnail()) {
        isCacheDirty = true;
        repaint();
    }
}

void WaveformView::setChannelMode(AppEnums::ChannelViewMode channelMode) {
    if (currentChannelMode == channelMode) return;
    currentChannelMode = channelMode;
    isCacheDirty = true;
    repaint();
}

void WaveformView::setActiveRegion(float startX, float endX) {
    if (activeStartX == startX && activeEndX == endX) return;
    activeStartX = startX;
    activeEndX = endX;
    repaint();
}

void WaveformView::paint(juce::Graphics &g) {
    if (isCacheDirty || cachedWaveformBright.getWidth() != getWidth() || cachedWaveformBright.getHeight() != getHeight()) {
        cachedWaveformBright = juce::Image(juce::Image::ARGB, juce::jmax(1, getWidth()), juce::jmax(1, getHeight()), true);
        cachedWaveformDark = juce::Image(juce::Image::ARGB, juce::jmax(1, getWidth()), juce::jmax(1, getHeight()), true);

        juce::Graphics igBright(cachedWaveformBright);
        juce::Graphics igDark(cachedWaveformDark);

        igBright.fillAll(Config::Colors::solidBlack);
        igDark.fillAll(Config::Colors::solidBlack);

        auto &thumbnail = waveformManager.getThumbnail();
        if (thumbnail.getTotalLength() > 0.0) {
            const auto bounds = getLocalBounds().toFloat();

            // Bake Bright Waveform
            juce::ColourGradient brightGrad(Config::Colors::waveformPeak, bounds.getX(), bounds.getY(),
                                            Config::Colors::waveformPeak, bounds.getX(), bounds.getBottom(), false);
            brightGrad.addColour(0.5, Config::Colors::waveformCore);
            igBright.setGradientFill(brightGrad);
            drawWaveform(igBright);

            // Bake Dark Waveform
            juce::ColourGradient darkGrad(Config::Colors::waveformPeak.withBrightness(0.1f), bounds.getX(), bounds.getY(),
                                          Config::Colors::waveformPeak.withBrightness(0.1f), bounds.getX(), bounds.getBottom(), false);
            darkGrad.addColour(0.5, Config::Colors::waveformCore.withBrightness(0.1f));
            igDark.setGradientFill(darkGrad);
            drawWaveform(igDark);
        }
        isCacheDirty = false;
    }

    // 1. Blast the dark waveform across the entire view
    g.drawImageAt(cachedWaveformDark, 0, 0);

    // 2. Hardware-clip and blast the bright waveform strictly inside the Cut Region bounds
    if (activeStartX < activeEndX) {
        g.saveState();
        g.reduceClipRegion((int)activeStartX, 0, (int)(activeEndX - activeStartX), getHeight());
        g.drawImageAt(cachedWaveformBright, 0, 0);
        g.restoreState();
    }
}

void WaveformView::drawWaveform(juce::Graphics &g) {
    const auto &thumbnail = waveformManager.getThumbnail();
    const auto totalLength = (float)thumbnail.getTotalLength();
    const auto bounds = getLocalBounds().toFloat();
    const auto width = bounds.getWidth();
    const auto centerY = bounds.getCentreY();
    const auto halfHeight = bounds.getHeight() * 0.5f;
    
    // Changed from Low (4) to High (1)
    const float step = (float)Config::Layout::Waveform::pixelsPerSampleHigh; 

    // Draw the full width into the cache
    for (float x = 0.0f; x < width; x += step) {
        const double startTime = CoordinateMapper::pixelsToSeconds(x, width, totalLength);
        const double endTime = CoordinateMapper::pixelsToSeconds(x + step, width, totalLength);

        float minVal = 0.0f, maxVal = 0.0f;
        thumbnail.getApproximateMinMax(startTime, endTime, 0, minVal, maxVal);

        const float top = centerY - (maxVal * halfHeight);
        const float bottom = centerY - (minVal * halfHeight);

        g.fillRect(x, top, step, juce::jmax(1.0f, bottom - top));
    }
}
