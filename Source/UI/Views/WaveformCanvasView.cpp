
#include "UI/Views/WaveformCanvasView.h"
#include "UI/ControlPanel.h"
#include "UI/Views/WaveformView.h"
#include "UI/Views/CutLayerView.h"
#include "UI/Views/PlaybackCursorView.h"
#include "UI/Views/ZoomView.h"

WaveformCanvasView::WaveformCanvasView(ControlPanel& owner) {
    setInterceptsMouseClicks(false, false);

    waveformView = std::make_unique<WaveformView>(owner.getAudioPlayer().getWaveformManager());
    addAndMakeVisible(waveformView.get());
    waveformView->setInterceptsMouseClicks(false, false);

    cutLayerView = std::make_unique<CutLayerView>(owner);
    addAndMakeVisible(cutLayerView.get());
    cutLayerView->setInterceptsMouseClicks(false, false);

    playbackCursorView = std::make_unique<PlaybackCursorView>(owner);
    addAndMakeVisible(playbackCursorView.get());
    playbackCursorView->setInterceptsMouseClicks(false, false);

    zoomView = std::make_unique<ZoomView>(owner);
    addAndMakeVisible(zoomView.get());
}

WaveformCanvasView::~WaveformCanvasView() = default;

void WaveformCanvasView::resized() {
    auto bounds = getLocalBounds();
    
    if (waveformView != nullptr) waveformView->setBounds(bounds);
    if (cutLayerView != nullptr) cutLayerView->setBounds(bounds);
    if (playbackCursorView != nullptr) playbackCursorView->setBounds(bounds);
    if (zoomView != nullptr) zoomView->setBounds(bounds);
}
