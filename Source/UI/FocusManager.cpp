

#include "UI/FocusManager.h"
#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"

FocusManager::FocusManager(ControlPanel &owner) : owner(owner) {
}

FocusTarget FocusManager::getCurrentTarget() const {
    const auto &markerMouse = owner.getMarkerMouseHandler();
    const auto &waveformMouse = owner.getWaveformMouseHandler();

    if (markerMouse.getDraggedHandle() == MarkerMouseHandler::CutMarkerHandle::In)
        return FocusTarget::CutIn;
    if (markerMouse.getDraggedHandle() == MarkerMouseHandler::CutMarkerHandle::Out)
        return FocusTarget::CutOut;

    if (waveformMouse.isScrubbing())
        return FocusTarget::MouseManual;

    const auto activePoint = owner.getInteractionCoordinator().getActiveZoomPoint();
    if (activePoint == AppEnums::ActiveZoomPoint::In)
        return FocusTarget::CutIn;
    if (activePoint == AppEnums::ActiveZoomPoint::Out)
        return FocusTarget::CutOut;

    return FocusTarget::Playback;
}

double FocusManager::getFocusedTime() const {
    FocusTarget target = getCurrentTarget();

    switch (target) {
    case FocusTarget::CutIn:
        return owner.getSessionState().getCutIn();
    case FocusTarget::CutOut:
        return owner.getSessionState().getCutOut();
    case FocusTarget::MouseManual:
    case FocusTarget::Playback:
    default:
        return owner.getAudioPlayer().getCurrentPosition();
    }
}

double FocusManager::getStepMultiplier(bool shift, bool ctrl) {
    if (shift && ctrl)
        return 0.01;
    if (shift)
        return 0.1;

    return 1.0;
}
