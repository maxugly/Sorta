/**
 * @file WaveformMouseHandler.cpp
 */

#include "UI/Handlers/WaveformMouseHandler.h"
#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/FocusManager.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "Utils/CoordinateMapper.h"
#include "Utils/Config.h"

WaveformMouseHandler::WaveformMouseHandler(ControlPanel &controlPanel) : owner(controlPanel) {
}

double WaveformMouseHandler::getMouseTime(int x, const juce::Rectangle<int> &bounds,
                                          double duration) const {
    if (duration <= 0.0)
        return 0.0;
    double rawTime = CoordinateMapper::pixelsToSeconds((float)(x - bounds.getX()),
                                                       (float)bounds.getWidth(), duration);

    double sampleRate = 0.0;
    juce::int64 length = 0;
    owner.getAudioPlayer().getReaderInfo(sampleRate, length);
    return owner.getInteractionCoordinator().getSnappedTime(rawTime, sampleRate);
}

void WaveformMouseHandler::mouseMove(const juce::MouseEvent &event) {
    const auto waveformBounds = owner.getWaveformBounds();
    if (waveformBounds.contains(event.getPosition())) {
        mouseCursorX = event.x;
        mouseCursorY = event.y;
        mouseCursorTime = getMouseTime(mouseCursorX, waveformBounds,
                                       owner.getAudioPlayer().getThumbnail().getTotalLength());
    } else {
        mouseCursorX = mouseCursorY = -1;
        mouseCursorTime = 0.0;
        isScrubbingState = false;
    }
}

void WaveformMouseHandler::mouseDown(const juce::MouseEvent &event) {
    clearTextEditorFocusIfNeeded(event);
    auto &coordinator = owner.getInteractionCoordinator();
    const double audioLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

    if (coordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None) {
        auto zb = coordinator.getZoomPopupBounds();
        if (zb.contains(event.getPosition())) {
            interactionStartedInZoom = true;
            auto tr = coordinator.getZoomTimeRange();
            double zoomedTime =
                CoordinateMapper::pixelsToSeconds((float)(event.x - zb.getX()),
                                                  (float)zb.getWidth(), tr.second - tr.first) +
                tr.first;

            if (event.mods.isLeftButtonDown()) {
                coordinator.setNeedsJumpToCutIn(true);
                const auto pm = coordinator.getPlacementMode();
                if (pm != AppEnums::PlacementMode::None) {
                    auto marker = (pm == AppEnums::PlacementMode::CutIn)
                                      ? AppEnums::ActiveZoomPoint::In
                                      : AppEnums::ActiveZoomPoint::Out;
                    coordinator.validateMarkerPosition(marker, zoomedTime, owner.getSessionState().getCutIn(),
                                                       owner.getSessionState().getCutOut(), audioLength);
                    if (pm == AppEnums::PlacementMode::CutIn) {
                        owner.getSessionState().setCutIn(zoomedTime);
                        owner.getSessionState().setAutoCutInActive(false);
                    } else {
                        owner.getSessionState().setCutOut(zoomedTime);
                        owner.getSessionState().setAutoCutOutActive(false);
                    }
                    return;
                } else {
                    const auto azp = coordinator.getActiveZoomPoint();
                    if (azp == AppEnums::ActiveZoomPoint::Playback) {
                        owner.getAudioPlayer().setPlayheadPosition(zoomedTime);
                        isDraggingFlag = isScrubbingState = true;
                        mouseDragStartX = event.x;
                        return;
                    }

                    double cpt = (azp == AppEnums::ActiveZoomPoint::In) ? owner.getSessionState().getCutIn()
                                                                        : owner.getSessionState().getCutOut();
                    float indicatorX = (float)zb.getX() + CoordinateMapper::secondsToPixels(
                                                              cpt - tr.first, (float)zb.getWidth(),
                                                              tr.second - tr.first);

                    if (std::abs(event.x - (int)indicatorX) >= Config::Layout::Glow::hitBoxTolerance) {
                        owner.getAudioPlayer().setPlayheadPosition(zoomedTime);
                        isDraggingFlag = isScrubbingState = true;
                        mouseDragStartX = event.x;
                        return;
                    }
                }
            }
        }
    }

    interactionStartedInZoom = false;
    const auto wb = owner.getWaveformBounds();
    if (!wb.contains(event.getPosition()))
        return;

    if (event.mods.isLeftButtonDown()) {
        if (!event.mods.isRightButtonDown()) {
            if (owner.getMarkerMouseHandler().getDraggedHandle() == MarkerMouseHandler::CutMarkerHandle::None) {
                isDraggingFlag = isScrubbingState = true;
                mouseDragStartX = event.x;
                seekToMousePosition(event.x);
            }
        }
    } else if (event.mods.isRightButtonDown()) {
        handleRightClickForCutPlacement(event.x);
    }
}

void WaveformMouseHandler::mouseDrag(const juce::MouseEvent &event) {
    if (!event.mods.isLeftButtonDown())
        return;
    const auto wb = owner.getWaveformBounds();
    const double audioLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    auto &coordinator = owner.getInteractionCoordinator();

    if (wb.contains(event.getPosition())) {
        mouseCursorX = event.x;
        mouseCursorY = event.y;
        mouseCursorTime = getMouseTime(event.x, wb, audioLength);
    }

    if (interactionStartedInZoom &&
        coordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None &&
        isDraggingFlag) {
        auto zb = coordinator.getZoomPopupBounds();
        auto tr = coordinator.getZoomTimeRange();
        int cx = juce::jlimit(zb.getX(), zb.getRight(), event.x);
        double zt = CoordinateMapper::pixelsToSeconds((float)(cx - zb.getX()), (float)zb.getWidth(),
                                                      tr.second - tr.first) +
                    tr.first;

        owner.getAudioPlayer().setPlayheadPosition(zt);
        return;
    }

    if (isDraggingFlag && wb.contains(event.getPosition())) {
        seekToMousePosition(event.x);
    }
}

void WaveformMouseHandler::mouseUp(const juce::MouseEvent &event) {
    auto &coordinator = owner.getInteractionCoordinator();
    if (coordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None &&
        (isDraggingFlag || coordinator.getPlacementMode() != AppEnums::PlacementMode::None)) {
        if (coordinator.getPlacementMode() != AppEnums::PlacementMode::None) {
            coordinator.setPlacementMode(AppEnums::PlacementMode::None);

        }
        isDraggingFlag = isScrubbingState = false;
        return;
    }

    if (isDraggingFlag) {
        isDraggingFlag = isScrubbingState = false;
        owner.getAudioPlayer().setPlayheadPosition(owner.getSessionState().getCutIn());
        owner.getInteractionCoordinator().setNeedsJumpToCutIn(false);
    }

    const auto wb = owner.getWaveformBounds();
    if (wb.contains(event.getPosition()) && event.mods.isLeftButtonDown()) {
        const auto pm = coordinator.getPlacementMode();
        if (pm != AppEnums::PlacementMode::None) {
            double t =
                getMouseTime(event.x, wb, owner.getAudioPlayer().getThumbnail().getTotalLength());
            auto marker = (pm == AppEnums::PlacementMode::CutIn) ? AppEnums::ActiveZoomPoint::In
                                                                 : AppEnums::ActiveZoomPoint::Out;
            coordinator.validateMarkerPosition(
                marker, t, owner.getSessionState().getCutIn(), owner.getSessionState().getCutOut(),
                owner.getAudioPlayer().getThumbnail().getTotalLength());
            if (pm == AppEnums::PlacementMode::CutIn) {
                owner.getSessionState().setCutIn(t);
                owner.getSessionState().setAutoCutInActive(false);
            } else {
                owner.getSessionState().setCutOut(t);
                owner.getSessionState().setAutoCutOutActive(false);
            }
            owner.getAudioPlayer().setPlayheadPosition(owner.getSessionState().getCutIn());
            owner.getInteractionCoordinator().setNeedsJumpToCutIn(false);
            coordinator.setPlacementMode(AppEnums::PlacementMode::None);

        } else if (mouseDragStartX == event.x) {
            seekToMousePosition(event.x);
        }
    }
}

void WaveformMouseHandler::mouseExit(const juce::MouseEvent &) {
    mouseCursorX = mouseCursorY = -1;
    mouseCursorTime = 0.0;
    isScrubbingState = false;
}

void WaveformMouseHandler::mouseWheelMove(const juce::MouseEvent &event,
                                          const juce::MouseWheelDetails &wheel) {
    const auto wb = owner.getWaveformBounds();
    if (!wb.contains(event.getPosition()))
        return;

    if (event.mods.isCtrlDown() && !event.mods.isShiftDown()) {
        owner.getInteractionCoordinator().handleMouseWheelZoom(wheel, owner.getSessionState());
        return;
    }

    double step =
        0.01 * FocusManager::getStepMultiplier(event.mods.isShiftDown(), event.mods.isCtrlDown());
    if (event.mods.isAltDown())
        step *= 10.0;
    owner.getAudioPlayer().setPlayheadPosition(owner.getAudioPlayer().getCurrentPosition() +
                                               ((wheel.deltaY > 0) ? 1.0 : -1.0) * step);
}

void WaveformMouseHandler::handleRightClickForCutPlacement(int x) {
    const auto wb = owner.getWaveformBounds();
    const double al = owner.getAudioPlayer().getThumbnail().getTotalLength();
    if (al <= 0.0)
        return;

    double t = getMouseTime(x, wb, al);
    auto &coordinator = owner.getInteractionCoordinator();
    const auto pm = coordinator.getPlacementMode();
    if (pm != AppEnums::PlacementMode::None) {
        auto m = (pm == AppEnums::PlacementMode::CutIn) ? AppEnums::ActiveZoomPoint::In
                                                        : AppEnums::ActiveZoomPoint::Out;
        coordinator.validateMarkerPosition(m, t, owner.getSessionState().getCutIn(),
                                           owner.getSessionState().getCutOut(), al);
        if (pm == AppEnums::PlacementMode::CutIn) {
            owner.getSessionState().setCutIn(t);
            owner.getSessionState().setAutoCutInActive(false);
        } else {
            owner.getSessionState().setCutOut(t);
            owner.getSessionState().setAutoCutOutActive(false);
        }
        coordinator.setPlacementMode(AppEnums::PlacementMode::None);

    }
}

void WaveformMouseHandler::seekToMousePosition(int x) {
    const auto wb = owner.getWaveformBounds();
    owner.getAudioPlayer().setPlayheadPosition(
        getMouseTime(x, wb, owner.getAudioPlayer().getThumbnail().getTotalLength()));
}

void WaveformMouseHandler::clearTextEditorFocusIfNeeded(const juce::MouseEvent &event) {
    const auto sp = event.getScreenPosition();
    for (int i = 0; i < owner.getNumChildComponents(); ++i) {
        auto *c = owner.getChildComponent(i);
        if (auto *e = dynamic_cast<juce::TextEditor *>(c)) {
            if (e->getScreenBounds().contains(sp))
                return;
        }
    }
    for (int i = 0; i < owner.getNumChildComponents(); ++i) {
        if (auto *e = dynamic_cast<juce::TextEditor *>(owner.getChildComponent(i))) {
            if (e->hasKeyboardFocus(false))
                e->giveAwayKeyboardFocus();
        }
    }
}
