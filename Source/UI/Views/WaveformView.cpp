#include "UI/Views/WaveformView.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"

WaveformView::WaveformView() {
    setInterceptsMouseClicks(false, false);
    setOpaque(true);
}

WaveformView::~WaveformView() = default;

void WaveformView::updateState(const WaveformViewState& newState) {
    if (state.thumbnail != newState.thumbnail ||
        state.totalLength != newState.totalLength ||
        state.channelMode != newState.channelMode ||
        (newState.thumbnail != nullptr && !newState.thumbnail->isFullyLoaded())) {
        isCacheDirty = true;
    }
    state = newState;
    repaint();
}

void WaveformView::clearCaches() {
    isCacheDirty = true;
    repaint();
}

void WaveformView::paint(juce::Graphics &g) {
    if (isCacheDirty || cachedWaveform.getWidth() != getWidth() || cachedWaveform.getHeight() != getHeight()) {
        cachedWaveform = juce::Image(juce::Image::ARGB, juce::jmax(1, getWidth()), juce::jmax(1, getHeight()), true);
        juce::Graphics ig(cachedWaveform);

        ig.fillAll(Config::Colors::solidBlack);
        
        if (state.thumbnail != nullptr && state.totalLength > 0.0) {
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
    if (state.thumbnail == nullptr) return;

    const auto totalLength = (float)state.totalLength;
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
        state.thumbnail->getApproximateMinMax(startTime, endTime, 0, minVal, maxVal);

        const float top = centerY - (maxVal * halfHeight);
        const float bottom = centerY - (minVal * halfHeight);

        g.fillRect(x, top, step, juce::jmax(1.0f, bottom - top));
    }
}
