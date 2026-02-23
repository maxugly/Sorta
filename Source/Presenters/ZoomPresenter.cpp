
#include "Presenters/ZoomPresenter.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "UI/ControlPanel.h"
#include "UI/FocusManager.h"
#include "UI/InteractionCoordinator.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/ZoomView.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"
#include "Utils/CoordinateMapper.h"

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

    ZoomViewState state;
    state.isZooming = isZooming;
    state.eyeCandyPulse = timerManager.getBreathingPulse();
    state.isZKeyDown = zDown;
    state.shouldShowEyeCandy = owner.getInteractionCoordinator().shouldShowEyeCandy();
    state.placementMode = owner.getInteractionCoordinator().getPlacementMode();
    state.channelMode = owner.getChannelViewMode();

    auto& audioPlayer = owner.getAudioPlayer();
    state.audioLength = audioPlayer.getWaveformManager().getThumbnail().getTotalLength();
    state.numChannels = audioPlayer.getWaveformManager().getThumbnail().getNumChannels();
    state.thumbnail = &audioPlayer.getWaveformManager().getThumbnail();

    const auto &markerMouse = owner.getMarkerMouseHandler();
    state.isDraggingCutIn = markerMouse.getDraggedHandle() == MarkerMouseHandler::CutMarkerHandle::In;
    state.isDraggingCutOut = markerMouse.getDraggedHandle() == MarkerMouseHandler::CutMarkerHandle::Out;

    if (currentMouseX != -1) {
        state.mouseX = currentMouseX - zoomView.getX();
        state.mouseY = currentMouseY - zoomView.getY();
        state.mouseTime = mouse.getMouseCursorTime();
        state.mouseTimeText = TimeUtils::formatTime(state.mouseTime);

        if (state.numChannels > 0) {
            double sampleRate = 0.0;
            juce::int64 length = 0;
            if (audioPlayer.getReaderInfo(sampleRate, length) && sampleRate > 0.0) {
                float minVal = 0.0f, maxVal = 0.0f;
                audioPlayer.getWaveformManager().getThumbnail().getApproximateMinMax(
                    state.mouseTime, state.mouseTime + (1.0 / sampleRate), 0,
                    minVal, maxVal);
                state.amplitude = juce::jmax(std::abs(minVal), std::abs(maxVal));
            }
        }
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

        if (state.audioLength > 0.0) {
            double zoomCenterTime = owner.getFocusManager().getFocusedTime();
            double timeRange = state.audioLength / (double)owner.getSessionState().getZoomFactor();
            timeRange = juce::jlimit(0.00005, state.audioLength, timeRange);

            state.startTime = zoomCenterTime - (timeRange / 2.0);
            state.endTime = state.startTime + timeRange;
            state.popupBounds = currentPopupBounds;

            auto &coordinator = owner.getInteractionCoordinator();
            coordinator.setZoomPopupBounds(currentPopupBounds.translated(zoomView.getX(), zoomView.getY()));
            coordinator.setZoomTimeRange(state.startTime, state.endTime);

            auto mapToPopup = [&](double time) {
                return (float)currentPopupBounds.getX() +
                       CoordinateMapper::secondsToPixels(time - state.startTime,
                                                         (float)currentPopupBounds.getWidth(), (double)timeRange);
            };

            state.cutInPixelX = mapToPopup(owner.getSessionState().getCutIn());
            state.cutOutPixelX = mapToPopup(owner.getSessionState().getCutOut());
            state.currentPositionPixelX = mapToPopup(audioPlayer.getCurrentPosition());
        }
    }

    zoomView.updateState(state);
}

void ZoomPresenter::animationUpdate(float breathingPulse) {
    juce::ignoreUnused(breathingPulse);
    playbackTimerTick();
}

void ZoomPresenter::activeZoomPointChanged(AppEnums::ActiveZoomPoint newPoint) {
    juce::ignoreUnused(newPoint);
    playbackTimerTick();
}
