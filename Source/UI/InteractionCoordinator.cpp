#include "UI/InteractionCoordinator.h"
#include "Core/SessionState.h"
#include "Utils/Config.h"
#include <algorithm>
#include <cmath>

void InteractionCoordinator::addListener(Listener *l) {
    listeners.add(l);
}

void InteractionCoordinator::removeListener(Listener *l) {
    listeners.remove(l);
}

double InteractionCoordinator::getSnappedTime(double rawTime, double sampleRate) const {
    if (sampleRate <= 0.0)
        return rawTime;

    // Snap to the nearest sample boundary
    const double samples = rawTime * sampleRate;
    return std::round(samples) / sampleRate;
}

void InteractionCoordinator::validateMarkerPosition(AppEnums::ActiveZoomPoint marker,
                                                    double &newPosition, double cutIn,
                                                    double cutOut, double duration) const {
    newPosition = std::clamp(newPosition, 0.0, duration);

    if (marker == AppEnums::ActiveZoomPoint::In) {
        // Rule: In <= Out
        newPosition = std::min(newPosition, cutOut);
    } else if (marker == AppEnums::ActiveZoomPoint::Out) {
        // Rule: Out >= In
        newPosition = std::max(newPosition, cutIn);
    }
}

void InteractionCoordinator::constrainFullRegionMove(double &newIn, double &newOut, double length,
                                                     double duration) const {
    if (newIn < 0.0) {
        newIn = 0.0;
        newOut = length;
    } else if (newOut > duration) {
        newOut = duration;
        newIn = duration - length;
    }
}

void InteractionCoordinator::handleMouseWheelZoom(const juce::MouseWheelDetails &wheel,
                                                  SessionState &state) const {
    const float step = (wheel.deltaY > 0) ? Config::Layout::Zoom::zoomStepIn
                                          : Config::Layout::Zoom::zoomStepOut;
    state.setZoomFactor(state.getZoomFactor() * step);
}
