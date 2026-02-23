#include "UI/Views/OverlayView.h"
#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"
#include <vector>

OverlayView::OverlayView(ControlPanel &ownerIn) : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

OverlayView::~OverlayView() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void OverlayView::animationUpdate(float breathingPulse) {
    juce::ignoreUnused(breathingPulse);
    if (owner.getInteractionCoordinator().shouldShowEyeCandy())
        repaint();
}

void OverlayView::paint(juce::Graphics &g) {
    auto &coordinator = owner.getInteractionCoordinator();
    if (!coordinator.shouldShowEyeCandy())
        return;

    g.setOpacity(1.0f);

    auto &audioPlayer = owner.getAudioPlayer();
    auto audioLength = audioPlayer.getThumbnail().getTotalLength();
    if (audioLength <= 0.0)
        return;

    const auto waveformBounds = owner.getWaveformBounds();
    const double cutIn = owner.getSessionState().getCutIn();
    const double cutOut = owner.getSessionState().getCutOut();

    float inX =
        (float)waveformBounds.getX() +
        CoordinateMapper::secondsToPixels(cutIn, (float)waveformBounds.getWidth(), audioLength);
    float outX =
        (float)waveformBounds.getX() +
        CoordinateMapper::secondsToPixels(cutOut, (float)waveformBounds.getWidth(), audioLength);

    const float pulse = owner.getPlaybackTimerManager().getBreathingPulse();
    const juce::Colour blueColor = Config::Colors::cutLine.withAlpha(0.5f + 0.3f * pulse);

    // Helper to map a rectangle from a nested child component to ControlPanel (our owner) space
    auto mapToLocal = [&](juce::Component *c, juce::Rectangle<int> r) {
        return getLocalArea(owner.getChildComponent(0), owner.getLocalArea(c, r));
    };

    // Actually, OverlayView is a child of ControlPanel and covers it.
    // So getLocalArea(owner, ...) is the correct way to map from owner-relative to
    // overlay-relative.
    auto mapFromOwner = [&](juce::Rectangle<int> r) { return getLocalArea(&owner, r); };

    auto mapComponentToLocal = [&](juce::Component *c) {
        return getLocalArea(c, c->getLocalBounds());
    };

    // 1. Draw connecting lines
    auto drawConnector = [&](float x, juce::Component *btn, bool toTop) {
        if (btn == nullptr)
            return;
        auto btnBounds = mapComponentToLocal(btn);
        float btnCenterX = (float)btnBounds.getCentreX();
        float btnBottomY = (float)btnBounds.getBottom();

        g.setColour(blueColor);
        g.drawLine(btnCenterX, btnBottomY, x,
                   toTop ? (float)waveformBounds.getY() : (float)waveformBounds.getBottom(),
                   Config::Layout::connectorLineWidth);
    };

    auto *inStrip = owner.getInStrip();
    auto *outStrip = owner.getOutStrip();

    if (inStrip != nullptr) {
        drawConnector(inX, &inStrip->getMarkerButton(), false);
        drawConnector(inX, &inStrip->getAutoCutButton(), true);
    }

    if (outStrip != nullptr) {
        drawConnector(outX, &outStrip->getMarkerButton(), false);
        drawConnector(outX, &outStrip->getAutoCutButton(), true);
    }

    // 2. Draw group outlines
    auto drawGroupOutline = [&](const std::vector<juce::Component *> &comps) {
        if (comps.empty())
            return;
        juce::Rectangle<int> groupBounds;
        bool first = true;
        for (auto *c : comps) {
            if (c == nullptr)
                continue;
            auto b = mapComponentToLocal(c);
            if (first) {
                groupBounds = b;
                first = false;
            } else {
                groupBounds = groupBounds.getUnion(b);
            }
        }

        if (!first) {
            g.setColour(blueColor);
            g.drawRect(groupBounds.expanded(Config::Layout::Stats::internalPadding + 1).toFloat(),
                       Config::Layout::connectorLineWidth);
        }
    };

    if (inStrip != nullptr) {
        std::vector<juce::Component *> inGroup = {
            &inStrip->getMarkerButton(), &inStrip->getTimerEditor(), &inStrip->getResetButton(),
            &owner.getSilenceDetector().getInSilenceThresholdEditor(),
            &inStrip->getAutoCutButton()};
        drawGroupOutline(inGroup);
    }

    if (outStrip != nullptr) {
        std::vector<juce::Component *> outGroup = {
            &outStrip->getMarkerButton(), &outStrip->getTimerEditor(), &outStrip->getResetButton(),
            &owner.getSilenceDetector().getOutSilenceThresholdEditor(),
            &outStrip->getAutoCutButton()};
        drawGroupOutline(outGroup);
    }
}
