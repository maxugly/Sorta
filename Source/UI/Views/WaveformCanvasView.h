#ifndef AUDIOFILER_WAVEFORMCANVASVIEW_H
#define AUDIOFILER_WAVEFORMCANVASVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <memory>

/**
 * @file WaveformCanvasView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Container component for the overlapping waveform display layers.
 */

class ControlPanel;
class WaveformView;
class CutLayerView;
class PlaybackCursorView;
class ZoomView;

/**
 * @class WaveformCanvasView
 * @brief A passive container that manages the perfectly overlapping layers of the waveform display.
 * 
 * @details Architecturally, the WaveformCanvasView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to bundle the WaveformView (audio data), 
 *          CutLayerView (markers), PlaybackCursorView (playhead), and ZoomView 
 *          (popups) into a single manageable unit. It ensures that all these 
 *          layers are precisely aligned and stacked correctly in the UI. 
 *          It relies entirely on its parent ControlPanel and the presenters 
 *          that manage its children to drive state updates.
 * 
 * @see WaveformView, CutLayerView, PlaybackCursorView, ZoomView, ControlPanel
 */
class WaveformCanvasView final : public juce::Component {
  public:
    /**
     * @brief Constructs a new WaveformCanvasView.
     * @param owner Reference to the parent ControlPanel.
     */
    explicit WaveformCanvasView(ControlPanel& owner);

    /** @brief Destructor. */
    ~WaveformCanvasView() override;

    /** @brief Standard JUCE resized callback, ensuring all child layers fill the canvas area. */
    void resized() override;

    /** @return Reference to the base waveform rendering view. */
    WaveformView& getWaveformView() { return *waveformView; }

    /** @return Reference to the cut boundary and marker overlay view. */
    CutLayerView& getCutLayerView() { return *cutLayerView; }

    /** @return Reference to the animated playback playhead view. */
    PlaybackCursorView& getPlaybackCursorView() { return *playbackCursorView; }

    /** @return Reference to the dynamic zoom preview popup view. */
    ZoomView& getZoomView() { return *zoomView; }

  private:
    std::unique_ptr<WaveformView> waveformView;
    std::unique_ptr<CutLayerView> cutLayerView;
    std::unique_ptr<PlaybackCursorView> playbackCursorView;
    std::unique_ptr<ZoomView> zoomView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformCanvasView)
};

#endif
