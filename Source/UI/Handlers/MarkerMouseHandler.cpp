/**
 * @file MarkerMouseHandler.cpp
 */

#include "UI/Handlers/MarkerMouseHandler.h"
#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/CoordinateMapper.h"
#include "Utils/Config.h"

MarkerMouseHandler::MarkerMouseHandler(ControlPanel &controlPanel) : owner(controlPanel) {
}

double MarkerMouseHandler::getMouseTime(int x, const juce::Rectangle<int> &bounds,
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

void MarkerMouseHandler::mouseMove(const juce::MouseEvent &event) {
    refreshHoverState(event);
}

void MarkerMouseHandler::refreshHoverState(const juce::MouseEvent &event) {
    if (event.mods.isAnyMouseButtonDown()) {
        hoveredHandle = CutMarkerHandle::None;
        return;
    }

    const auto waveformBounds = owner.getWaveformBounds();
    if (waveformBounds.contains(event.getPosition())) {
        hoveredHandle = getHandleAtPosition(event.getPosition());

        if (Config::Audio::lockHandlesWhenAutoCutActive) {
            const auto &autoCut = owner.getSessionState().getCutPrefs().autoCut;
            if ((hoveredHandle == CutMarkerHandle::In && autoCut.inActive) ||
                (hoveredHandle == CutMarkerHandle::Out && autoCut.outActive) ||
                (hoveredHandle == CutMarkerHandle::Full &&
                 (autoCut.inActive || autoCut.outActive))) {
                hoveredHandle = CutMarkerHandle::None;
            }
        }
    } else {
        hoveredHandle = CutMarkerHandle::None;
    }
}

void MarkerMouseHandler::mouseDown(const juce::MouseEvent &event) {
    auto &coordinator = owner.getInteractionCoordinator();
    const double audioLength = owner.getAudioPlayer().getThumbnail().getTotalLength();

    if (coordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None) {
        auto zb = coordinator.getZoomPopupBounds();
        if (zb.contains(event.getPosition())) {
            auto tr = coordinator.getZoomTimeRange();
            double zoomedTime =
                CoordinateMapper::pixelsToSeconds((float)(event.x - zb.getX()),
                                                  (float)zb.getWidth(), tr.second - tr.first) +
                tr.first;

            if (event.mods.isLeftButtonDown()) {
                const auto pm = coordinator.getPlacementMode();
                if (pm == AppEnums::PlacementMode::None) {
                    const auto azp = coordinator.getActiveZoomPoint();
                    double cpt = (azp == AppEnums::ActiveZoomPoint::In) ? owner.getSessionState().getCutIn()
                                                                        : owner.getSessionState().getCutOut();
                    float indicatorX = (float)zb.getX() + CoordinateMapper::secondsToPixels(
                                                              cpt - tr.first, (float)zb.getWidth(),
                                                              tr.second - tr.first);

                    if (std::abs(event.x - (int)indicatorX) < 20) {
                        draggedHandle = (azp == AppEnums::ActiveZoomPoint::In)
                                            ? CutMarkerHandle::In
                                            : CutMarkerHandle::Out;
                        dragStartMouseOffset = zoomedTime - cpt;
                        if (draggedHandle == CutMarkerHandle::In)
                            owner.getSessionState().setAutoCutInActive(false);
                        else
                            owner.getSessionState().setAutoCutOutActive(false);
                        return;
                    }
                }
            }
        }
    }

    const auto wb = owner.getWaveformBounds();
    if (!wb.contains(event.getPosition()))
        return;

    if (event.mods.isLeftButtonDown()) {
        draggedHandle = getHandleAtPosition(event.getPosition());
        const auto &autoCut = owner.getSessionState().getCutPrefs().autoCut;

        if (Config::Audio::lockHandlesWhenAutoCutActive &&
            ((draggedHandle == CutMarkerHandle::In && autoCut.inActive) ||
             (draggedHandle == CutMarkerHandle::Out && autoCut.outActive) ||
             (draggedHandle == CutMarkerHandle::Full &&
              (autoCut.inActive || autoCut.outActive)))) {
            draggedHandle = CutMarkerHandle::None;
        }

        if (draggedHandle != CutMarkerHandle::None) {
            if (draggedHandle == CutMarkerHandle::In || draggedHandle == CutMarkerHandle::Full)
                owner.getSessionState().setAutoCutInActive(false);
            if (draggedHandle == CutMarkerHandle::Out || draggedHandle == CutMarkerHandle::Full)
                owner.getSessionState().setAutoCutOutActive(false);

            if (draggedHandle == CutMarkerHandle::Full) {
                dragStartCutLength = std::abs(owner.getSessionState().getCutOut() - owner.getSessionState().getCutIn());
                dragStartMouseOffset =
                    getMouseTime(event.x, wb, audioLength) - owner.getSessionState().getCutIn();
            }
        }
    }
}

void MarkerMouseHandler::mouseDrag(const juce::MouseEvent &event) {
    refreshHoverState(event);
    if (!event.mods.isLeftButtonDown())
        return;
    const auto wb = owner.getWaveformBounds();
    const double audioLength = owner.getAudioPlayer().getThumbnail().getTotalLength();
    auto &coordinator = owner.getInteractionCoordinator();

    if (draggedHandle != CutMarkerHandle::None) {
        if (coordinator.getActiveZoomPoint() != AppEnums::ActiveZoomPoint::None &&
            coordinator.getZoomPopupBounds().contains(event.getPosition())) {
            auto zb = coordinator.getZoomPopupBounds();
            auto tr = coordinator.getZoomTimeRange();
            int cx = juce::jlimit(zb.getX(), zb.getRight(), event.x);
            double zt = CoordinateMapper::pixelsToSeconds((float)(cx - zb.getX()), (float)zb.getWidth(),
                                                          tr.second - tr.first) +
                        tr.first;

            double offset = (coordinator.getPlacementMode() == AppEnums::PlacementMode::None)
                                ? dragStartMouseOffset
                                : 0.0;
            double tt = zt - offset;
            auto marker = (draggedHandle == CutMarkerHandle::In) ? AppEnums::ActiveZoomPoint::In
                                                                 : AppEnums::ActiveZoomPoint::Out;
            coordinator.validateMarkerPosition(marker, tt, owner.getSessionState().getCutIn(),
                                               owner.getSessionState().getCutOut(), audioLength);
            if (draggedHandle == CutMarkerHandle::In)
                owner.getAudioPlayer().setCutIn(tt);
            else
                owner.getAudioPlayer().setCutOut(tt);
        } else {
            double mt = getMouseTime(juce::jlimit(wb.getX(), wb.getRight(), event.x), wb, audioLength);
            if (draggedHandle == CutMarkerHandle::Full) {
                double ni = mt - dragStartMouseOffset, no = ni + dragStartCutLength;
                coordinator.constrainFullRegionMove(ni, no, dragStartCutLength, audioLength);
                owner.getAudioPlayer().setCutIn(ni);
                owner.getAudioPlayer().setCutOut(no);
                owner.getAudioPlayer().setPlayheadPosition(owner.getAudioPlayer().getCurrentPosition());
            } else {
                auto marker = (draggedHandle == CutMarkerHandle::In) ? AppEnums::ActiveZoomPoint::In
                                                                     : AppEnums::ActiveZoomPoint::Out;
                coordinator.validateMarkerPosition(marker, mt, owner.getSessionState().getCutIn(),
                                                   owner.getSessionState().getCutOut(), audioLength);
                if (draggedHandle == CutMarkerHandle::In)
                    owner.getAudioPlayer().setCutIn(mt);
                else
                    owner.getAudioPlayer().setCutOut(mt);
            }
        }
    }
}

void MarkerMouseHandler::mouseUp(const juce::MouseEvent &event) {
    draggedHandle = CutMarkerHandle::None;
    refreshHoverState(event);
}

void MarkerMouseHandler::mouseExit(const juce::MouseEvent &) {
    hoveredHandle = CutMarkerHandle::None;
}

bool MarkerMouseHandler::isHandleActive(CutMarkerHandle h) const {
    if (draggedHandle == h || hoveredHandle == h)
        return true;
    auto &c = owner.getInteractionCoordinator();
    return (h == CutMarkerHandle::In && c.getPlacementMode() == AppEnums::PlacementMode::CutIn) ||
           (h == CutMarkerHandle::Out && c.getPlacementMode() == AppEnums::PlacementMode::CutOut);
}

MarkerMouseHandler::CutMarkerHandle MarkerMouseHandler::getHandleAtPosition(juce::Point<int> pos) const {
    const auto wb = owner.getWaveformBounds();
    const double al = owner.getAudioPlayer().getThumbnail().getTotalLength();
    if (al <= 0.0)
        return CutMarkerHandle::None;

    auto check = [&](double t) {
        float x = (float)wb.getX() + CoordinateMapper::secondsToPixels(t, (float)wb.getWidth(), al);
        return juce::Rectangle<int>((int)(x - Config::Layout::Glow::cutMarkerBoxWidth / 2.0f),
                                    wb.getY(), (int)Config::Layout::Glow::cutMarkerBoxWidth,
                                    wb.getHeight())
            .contains(pos);
    };

    if (check(owner.getSessionState().getCutIn()))
        return CutMarkerHandle::In;
    if (check(owner.getSessionState().getCutOut()))
        return CutMarkerHandle::Out;

    const double actualIn = juce::jmin(owner.getSessionState().getCutIn(), owner.getSessionState().getCutOut()),
                 actualOut = juce::jmax(owner.getSessionState().getCutIn(), owner.getSessionState().getCutOut());
    float inX = (float)wb.getX() +
                CoordinateMapper::secondsToPixels(actualIn, (float)wb.getWidth(), al),
          outX = (float)wb.getX() +
                 CoordinateMapper::secondsToPixels(actualOut, (float)wb.getWidth(), al);
    int hh = Config::Layout::Glow::cutMarkerBoxHeight;
    if (juce::Rectangle<int>((int)inX, wb.getY(), (int)(outX - inX), hh).contains(pos) ||
        juce::Rectangle<int>((int)inX, wb.getBottom() - hh, (int)(outX - inX), hh).contains(pos))
        return CutMarkerHandle::Full;

    return CutMarkerHandle::None;
}
