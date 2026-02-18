#include "WaveformView.h"

#include "WaveformManager.h"
#include <cmath>

WaveformView::WaveformView(WaveformManager& waveformManagerIn)
    : waveformManager(waveformManagerIn)
{
    waveformManager.addChangeListener(this);
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

WaveformView::~WaveformView()
{
    waveformManager.removeChangeListener(this);
}

void WaveformView::invalidateWaveformCache()
{
    waveformCache = juce::Image();
    lastAudioLength = -1.0;
}

void WaveformView::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &waveformManager.getThumbnail())
    {
        invalidateWaveformCache();
        repaint();
    }
}

void WaveformView::setQuality(AppEnums::ThumbnailQuality quality)
{
    if (currentQuality == quality)
        return;
    currentQuality = quality;
    invalidateWaveformCache();
    repaint();
}

void WaveformView::setChannelMode(AppEnums::ChannelViewMode channelMode)
{
    if (currentChannelMode == channelMode)
        return;
    currentChannelMode = channelMode;
    invalidateWaveformCache();
    repaint();
}

void WaveformView::paint(juce::Graphics& g)
{
    drawWaveform(g, getLocalBounds());
}

void WaveformView::drawWaveform(juce::Graphics& g,
                                const juce::Rectangle<int>& bounds) const
{
    auto& thumbnail = waveformManager.getThumbnail();
    const auto numChannels = thumbnail.getNumChannels();
    if (numChannels <= 0)
        return;

    const double audioLength = thumbnail.getTotalLength();
    const float scale = g.getInternalContext().getPhysicalPixelScaleFactor();

    if (!waveformCache.isValid()
        || lastBounds != bounds
        || std::abs(lastAudioLength - audioLength) > 0.001
        || lastQuality != static_cast<int>(currentQuality)
        || lastChannelMode != static_cast<int>(currentChannelMode)
        || std::abs(lastScale - scale) > 0.001f)
    {
        const int w = juce::roundToInt((float)bounds.getWidth() * scale);
        const int h = juce::roundToInt((float)bounds.getHeight() * scale);

        if (w <= 0 || h <= 0)
            return;

        waveformCache = juce::Image(juce::Image::ARGB, w, h, true);
        juce::Graphics ig(waveformCache);

        ig.addTransform(juce::AffineTransform::scale(scale));
        ig.setOrigin(-bounds.getX(), -bounds.getY());

        int pixelsPerSample = 1;
        if (currentQuality == AppEnums::ThumbnailQuality::Low)
            pixelsPerSample = Config::Layout::Waveform::pixelsPerSampleLow;
        else if (currentQuality == AppEnums::ThumbnailQuality::Medium)
            pixelsPerSample = Config::Layout::Waveform::pixelsPerSampleMedium;

        ig.setColour(Config::Colors::waveform);
        if (currentChannelMode == AppEnums::ChannelViewMode::Mono || numChannels == 1)
        {
            if (pixelsPerSample > 1)
                drawReducedQualityWaveform(ig, thumbnail, bounds, 0, pixelsPerSample);
            else
                thumbnail.drawChannel(ig, bounds, 0.0, audioLength, 0, 1.0f);
        }
        else
        {
            if (pixelsPerSample > 1)
            {
                for (int ch = 0; ch < numChannels; ++ch)
                    drawReducedQualityWaveform(ig, thumbnail, bounds, ch, pixelsPerSample);
            }
            else
            {
                thumbnail.drawChannels(ig, bounds, 0.0, audioLength, 1.0f);
            }
        }

        lastBounds = bounds;
        lastAudioLength = audioLength;
        lastQuality = static_cast<int>(currentQuality);
        lastChannelMode = static_cast<int>(currentChannelMode);
        lastScale = scale;
    }

    g.drawImage(waveformCache, bounds.toFloat());
}

void WaveformView::drawReducedQualityWaveform(juce::Graphics& g,
                                              juce::AudioThumbnail& thumbnail,
                                              const juce::Rectangle<int>& bounds,
                                              int channel,
                                              int pixelsPerSample) const
{
    const auto audioLength = thumbnail.getTotalLength();
    if (audioLength <= 0.0)
        return;

    const int width = bounds.getWidth();
    if (width <= 0)
        return;

    const float height = (float)bounds.getHeight();
    const float centerY = (float)bounds.getCentreY();
    const float halfHeightScale = height * Config::Layout::Waveform::heightScale;
    if (pixelsPerSample <= 0)
        return;
    const double timePerPixel = audioLength / (double)width;
    const double timeDelta = timePerPixel * pixelsPerSample;
    const int offsetX = bounds.getX();

    juce::RectangleList<float> waveformRects;

    for (int x = 0; x < width; x += pixelsPerSample)
    {
        const double time = (double)x * timePerPixel;
        float minVal = 0.0f, maxVal = 0.0f;
        thumbnail.getApproximateMinMax(time, time + timeDelta, channel, minVal, maxVal);

        const float topY = centerY - (maxVal * halfHeightScale);
        const float bottomY = centerY - (minVal * halfHeightScale);

        const float xPos = (float)(offsetX + x);
        waveformRects.ensureStorageAllocated(width / pixelsPerSample + 1);
        waveformRects.addWithoutMerging({ xPos, topY, 1.0f, bottomY - topY });
    }

    g.fillRectList(waveformRects);
}
