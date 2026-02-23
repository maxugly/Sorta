
#include "Presenters/ZoomPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/ZoomView.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "UI/InteractionCoordinator.h"
#include "UI/FocusManager.h"
#include "Core/AudioPlayer.h"
#include "Utils/Config.h"

ZoomPresenter::ZoomPresenter(ControlPanel& ownerIn) : owner(ownerIn) {
}

ZoomPresenter::~ZoomPresenter() = default;

void ZoomPresenter::playbackTimerTick() {
    auto& zoomView = owner.waveformCanvasView->getZoomView();
    const auto &mouse = owner.getWaveformMouseHandler();
    const int currentMouseX = mouse.getMouseCursorX();
    const int currentMouseY = mouse.getMouseCursorY();

    const auto &timerManager = owner.getPlaybackTimerManager();
    const bool zDown = timerManager.isZKeyDown();
    const auto activePoint = owner.getInteractionCoordinator().getActiveZoomPoint();
    const bool isZooming = zDown || activePoint != AppEnums::ActiveZoomPoint::None;

    if (currentMouseX != lastMouseX || currentMouseY != lastMouseY) {
        if (lastMouseX != -1) {
            zoomView.repaint(lastMouseX - 1, 0, 3, zoomView.getHeight());
            zoomView.repaint(0, lastMouseY - 1, zoomView.getWidth(), 3);
        }

        if (currentMouseX != -1) {
            zoomView.repaint(currentMouseX - 1, 0, 3, zoomView.getHeight());
            zoomView.repaint(0, currentMouseY - 1, zoomView.getWidth(), 3);
        }

        lastMouseX = currentMouseX;
        lastMouseY = currentMouseY;
    }

    if (isZooming) {
        const auto waveformBounds = zoomView.getLocalBounds();
        const int popupWidth =
            juce::roundToInt((float)waveformBounds.getWidth() * Config::Layout::Zoom::popupScale);
        const int popupHeight =
            juce::roundToInt((float)waveformBounds.getHeight() * Config::Layout::Zoom::popupScale);
        const juce::Rectangle<int> currentPopupBounds(waveformBounds.getCentreX() - popupWidth / 2,
                                                      waveformBounds.getCentreY() - popupHeight / 2,
                                                      popupWidth, popupHeight);

        // Calculate time range for ZoomView::paint
        auto &audioPlayer = owner.getAudioPlayer();
        const double audioLength = audioPlayer.getWaveformManager().getThumbnail().getTotalLength();
        if (audioLength > 0.0) {
            double zoomCenterTime = owner.getFocusManager().getFocusedTime();
            double timeRange = audioLength / (double)owner.getZoomFactor();
            timeRange = juce::jlimit(0.00005, audioLength, timeRange);

            const double startTime = zoomCenterTime - (timeRange / 2.0);
            const double endTime = startTime + timeRange;

            auto &coordinator = owner.getInteractionCoordinator();
            coordinator.setZoomPopupBounds(currentPopupBounds.translated(zoomView.getX(), zoomView.getY()));
            coordinator.setZoomTimeRange(startTime, endTime);
        }

        if (currentPopupBounds != lastPopupBounds) {
            zoomView.repaint(lastPopupBounds.expanded(5));
            zoomView.repaint(currentPopupBounds.expanded(5));
            lastPopupBounds = currentPopupBounds;
        } else {
            zoomView.repaint(currentPopupBounds.expanded(5));
        }
    } else if (!lastPopupBounds.isEmpty()) {
        zoomView.repaint(lastPopupBounds.expanded(5));
        lastPopupBounds = juce::Rectangle<int>();
    }
}

void ZoomPresenter::animationUpdate(float breathingPulse) {
    juce::ignoreUnused(breathingPulse);
    owner.waveformCanvasView->getZoomView().repaint();
}

void ZoomPresenter::activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) {
    juce::ignoreUnused(newPoint);
    owner.waveformCanvasView->getZoomView().repaint();
}
