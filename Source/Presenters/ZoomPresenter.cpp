
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
        // currentMouseX/Y are relative to ControlPanel.
        // zoomView is a child of waveformCanvasView which is a child of ControlPanel.
        state.mouseX = currentMouseX - owner.waveformCanvasView->getX() - zoomView.getX();
        state.mouseY = currentMouseY - owner.waveformCanvasView->getY() - zoomView.getY();
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
                const float amplitude = juce::jmax(std::abs(minVal), std::abs(maxVal));
                const auto waveformBounds = owner.getWaveformBounds();
                const float centerY = (float)waveformBounds.getHeight() / 2.0f;
                state.amplitudeY = centerY - (amplitude * waveformBounds.getHeight() *
                                              Config::Layout::Waveform::heightScale);
                state.bottomAmplitudeY = centerY + (amplitude * waveformBounds.getHeight() *
                                                    Config::Layout::Waveform::heightScale);
                state.amplitudeText = juce::String(amplitude, 2);
                state.negAmplitudeText = juce::String(-amplitude, 2);
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

        const auto mods = juce::ModifierKeys::getCurrentModifiers();
        const bool isShift = mods.isShiftDown() && !mods.isCtrlDown();
        const bool isAlt = mods.isAltDown();
        const bool isCtrlShift = mods.isCtrlDown() && mods.isShiftDown();
        const bool isDefault = !isShift && !isAlt && !isCtrlShift;

        state.hudLines.push_back({Config::Labels::zoomPrefix + juce::String(owner.getSessionState().getZoomFactor(), 1) + "x", true});
        state.hudLines.push_back({Config::Labels::stepDefault, isDefault});
        state.hudLines.push_back({Config::Labels::stepShift, isShift});
        state.hudLines.push_back({Config::Labels::stepAlt, isAlt});
        state.hudLines.push_back({Config::Labels::stepCtrlShift, isCtrlShift});
    }

    // --- NEW LOGIC: Pre-calculate visual properties ---
    state.cursorLineColor = Config::Colors::mouseCursorLine;
    state.cursorHighlightColor = Config::Colors::mouseCursorHighlight;
    state.cursorGlowColor = Config::Colors::mouseAmplitudeGlow;
    state.cursorGlowThickness = Config::Layout::Glow::thickness;

    if (state.placementMode == AppEnums::PlacementMode::CutIn ||
        state.placementMode == AppEnums::PlacementMode::CutOut) {
        state.cursorLineColor = Config::Colors::mousePlacementMode;
        state.cursorHighlightColor = Config::Colors::mousePlacementMode.withAlpha(0.4f);
        state.cursorGlowColor = Config::Colors::placementModeGlow;
        state.cursorGlowThickness = Config::Layout::Glow::placementModeGlowThickness;
    } else if (state.isZKeyDown) {
        state.cursorLineColor = Config::Colors::mousePlacementMode;
        state.cursorHighlightColor = Config::Colors::mousePlacementMode.withAlpha(0.4f);
    }
    // ---------------------------------------------------

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
