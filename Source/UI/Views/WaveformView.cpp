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

void WaveformView::paint(juce::Graphics &g) {
    if (isCacheDirty || cachedWaveform.getWidth() != getWidth() || cachedWaveform.getHeight() != getHeight()) {
        cachedWaveform = juce::Image(juce::Image::ARGB, juce::jmax(1, getWidth()), juce::jmax(1, getHeight()), true);
        juce::Graphics ig(cachedWaveform);

        ig.fillAll(Config::Colors::solidBlack);
        auto &thumbnail = waveformManager.getThumbnail();
        
        if (thumbnail.getTotalLength() > 0.0) {
            const auto bounds = getLocalBounds().toFloat();
            juce::ColourGradient gradient(Config::Colors::waveformPeak, bounds.getX(), bounds.getY(),
                                          Config::Colors::waveformPeak, bounds.getX(), bounds.getBottom(), false);
            gradient.addColour(0.5, Config::Colors::waveformCore);
            ig.setGradientFill(gradient);
            drawWaveform(ig);
        }
        isCacheDirty = false;
    }
    g.drawImageAt(cachedWaveform, 0, 0);
}

void WaveformView::drawWaveform(juce::Graphics &g) {
    const auto &thumbnail = waveformManager.getThumbnail();
    const auto totalLength = (float)thumbnail.getTotalLength();
    const auto bounds = getLocalBounds().toFloat();
    const auto width = bounds.getWidth();
    const auto centerY = bounds.getCentreY();
    const auto halfHeight = bounds.getHeight() * 0.5f;
    const float step = (float)Config::Layout::Waveform::pixelsPerSampleLow;

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
