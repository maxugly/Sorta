
#ifndef AUDIOFILER_WAVEFORMCANVASVIEW_H
#define AUDIOFILER_WAVEFORMCANVASVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <memory>

class ControlPanel;
class WaveformView;
class CutLayerView;
class PlaybackCursorView;
class ZoomView;

/**
 * @file WaveformCanvasView.h
 * @ingroup UI
 * @brief A passive container for the overlapping waveform display layers.
 * @details This component bundles WaveformView, CutLayerView, PlaybackCursorView,
 *          and ZoomView into a single manageable unit. It handles the layout 
 *          (perfect overlap) of these layers but contains no business logic.
 *
 * @see ControlPanel
 */
class WaveformCanvasView final : public juce::Component {
  public:
    explicit WaveformCanvasView(ControlPanel& owner);
    ~WaveformCanvasView() override;

    void resized() override;

    WaveformView& getWaveformView() { return *waveformView; }
    CutLayerView& getCutLayerView() { return *cutLayerView; }
    PlaybackCursorView& getPlaybackCursorView() { return *playbackCursorView; }
    ZoomView& getZoomView() { return *zoomView; }

  private:
    std::unique_ptr<WaveformView> waveformView;
    std::unique_ptr<CutLayerView> cutLayerView;
    std::unique_ptr<PlaybackCursorView> playbackCursorView;
    std::unique_ptr<ZoomView> zoomView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformCanvasView)
};

#endif
