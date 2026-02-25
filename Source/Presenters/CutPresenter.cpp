

#include "Presenters/CutPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/CutLayerView.h"
#include "UI/InteractionCoordinator.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Core/WaveformManager.h"
#include "Utils/CoordinateMapper.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"

CutPresenter::CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                           CutLayerView &cutLayerViewIn,
                           InteractionCoordinator &interactionCoordinatorIn,
                           PlaybackTimerManager &playbackTimerManagerIn)
    : sessionState(sessionStateIn), cutLayerView(cutLayerViewIn),
      interactionCoordinator(interactionCoordinatorIn),
      playbackTimerManager(playbackTimerManagerIn) {
    markerMouseHandler = std::make_unique<MarkerMouseHandler>(controlPanel);
    waveformMouseHandler = std::make_unique<WaveformMouseHandler>(controlPanel);
    sessionState.addListener(this);
    playbackTimerManager.addListener(this);

    refreshMarkersVisibility();
    pushStateToView();
}

CutPresenter::~CutPresenter() {
    playbackTimerManager.removeListener(this);
    sessionState.removeListener(this);
}

void CutPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &) {
    refreshMarkersVisibility();
    pushStateToView();
}

void CutPresenter::playbackTimerTick() {
    pushStateToView();
}

void CutPresenter::updateAnimationState(CutLayerState& state) {
    // Also update marker interaction states high-frequency
    auto calcMarkerProps = [&](MarkerMouseHandler::CutMarkerHandle handle, bool isAuto) {
        juce::Colour color = isAuto ? Config::Colors::cutMarkerAuto : Config::Colors::cutLine;
        float thickness = Config::Layout::Glow::cutBoxOutlineThickness;
        bool pulse = false;

        const auto dragged = markerMouseHandler->getDraggedHandle();
        const auto hovered = markerMouseHandler->getHoveredHandle();

        // Exact same logic as buttons: you are either being dragged (Down) or hovered (Over).
        // If we are dragging something, ONLY that thing gets the drag color.
        // If we are not dragging, ONLY the thing being hovered gets the hover color.
        
        if (dragged != MarkerMouseHandler::CutMarkerHandle::None) {
            if (dragged == handle) {
                color = Config::Colors::cutMarkerDrag;
                thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
            }
        } else if (hovered == handle) {
            color = Config::Colors::cutMarkerHover;
            thickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
        }

        return std::make_tuple(color, thickness, pulse);
    };

    auto inProps = calcMarkerProps(MarkerMouseHandler::CutMarkerHandle::In, sessionState.getCutPrefs().autoCut.inActive);
    state.inMarkerColor = std::get<0>(inProps);
    state.inMarkerThickness = std::get<1>(inProps);
    state.inMarkerShouldPulse = std::get<2>(inProps);

    auto outProps = calcMarkerProps(MarkerMouseHandler::CutMarkerHandle::Out, sessionState.getCutPrefs().autoCut.outActive);
    state.outMarkerColor = std::get<0>(outProps);
    state.outMarkerThickness = std::get<1>(outProps);
    state.outMarkerShouldPulse = std::get<2>(outProps);

    const auto dragged = markerMouseHandler->getDraggedHandle();
    const auto hovered = markerMouseHandler->getHoveredHandle();
    
    state.regionOutlineColor = Config::Colors::cutLine;
    state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThickness;
    state.regionShouldPulse = false;

    if (dragged == MarkerMouseHandler::CutMarkerHandle::Full) {
        state.regionOutlineColor = Config::Colors::cutMarkerDrag;
        state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    } else if (dragged == MarkerMouseHandler::CutMarkerHandle::None && hovered == MarkerMouseHandler::CutMarkerHandle::Full) {
        state.regionOutlineColor = Config::Colors::cutMarkerHover;
        state.regionOutlineThickness = Config::Layout::Glow::cutBoxOutlineThicknessInteracting;
    }

    cutLayerView.updateState(state);
}

void CutPresenter::pushStateToView() {
    CutLayerState state;
    const double cutIn = sessionState.getCutIn();
    const double cutOut = sessionState.getCutOut();
    const double audioLength = cutLayerView.getOwner().getAudioPlayer().getThumbnail().getTotalLength();
    const auto bounds = cutLayerView.getLocalBounds();
    const float viewWidth = (float)bounds.getWidth();

    state.inPixelX = (float)bounds.getX() + CoordinateMapper::secondsToPixels(cutIn, viewWidth, audioLength);
    state.outPixelX = (float)bounds.getX() + CoordinateMapper::secondsToPixels(cutOut, viewWidth, audioLength);

    const float inX = juce::jlimit((float)bounds.getX(), (float)bounds.getRight(), state.inPixelX);
    const float outX = juce::jlimit((float)bounds.getX(), (float)bounds.getRight(), state.outPixelX);

    state.actualInX = juce::jmin(inX, outX);
    state.actualOutX = juce::jmax(inX, outX);

    auto calcThresholdY = [&](float threshold) {
        const float centerY = (float)bounds.getCentreY();
        const float halfHeight = (float)bounds.getHeight() / 2.0f;
        float top = centerY - (threshold * halfHeight);
        float bottom = centerY + (threshold * halfHeight);
        return std::make_pair(juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), top),
                              juce::jlimit((float)bounds.getY(), (float)bounds.getBottom(), bottom));
    };

    auto inY = calcThresholdY(sessionState.getCutPrefs().autoCut.thresholdIn);
    state.inThresholdYTop = inY.first;
    state.inThresholdYBottom = inY.second;

    auto outY = calcThresholdY(sessionState.getCutPrefs().autoCut.thresholdOut);
    state.outThresholdYTop = outY.first;
    state.outThresholdYBottom = outY.second;

    state.audioLength = (float)audioLength;
    state.markersVisible = sessionState.getCutPrefs().active;
    state.channelMode = cutLayerView.getOwner().getChannelViewMode();

    updateAnimationState(state);
}

void CutPresenter::refreshMarkersVisibility() {
    // This is now handled via pushStateToView and state.markersVisible
}
