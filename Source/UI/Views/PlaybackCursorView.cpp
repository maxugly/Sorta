

#include "UI/Views/PlaybackCursorView.h"
#include "Core/AudioPlayer.h"
#include "Core/WaveformManager.h"
#include "UI/ControlPanel.h"
#include "UI/Views/PlaybackCursorGlow.h"
#include "Utils/Config.h"
#include "Utils/CoordinateMapper.h"

PlaybackCursorView::PlaybackCursorView(ControlPanel &ownerPanel) : owner(ownerPanel) {
    setInterceptsMouseClicks(false, false);

    setOpaque(false);
}

PlaybackCursorView::~PlaybackCursorView() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void PlaybackCursorView::playbackTimerTick() {
    const auto &audioPlayer = owner.getAudioPlayer();
    const double audioLength = audioPlayer.getWaveformManager().getThumbnail().getTotalLength();
    if (audioLength > 0.0) {
        const auto &layout = owner.getWaveformBounds();
        const float x = CoordinateMapper::secondsToPixels(audioPlayer.getCurrentPosition(),
                                                          (float)layout.getWidth(), audioLength);
        const int currentX = juce::roundToInt(x);

        if (currentX != lastCursorX) {
            const int glowWidth = juce::roundToInt(Config::Layout::Glow::thickness) + 1;
            if (lastCursorX >= 0)
                repaint(lastCursorX - glowWidth, 0, glowWidth * 2, getHeight());

            repaint(currentX - glowWidth, 0, glowWidth * 2, getHeight());

            lastCursorX = currentX;
        }

        const auto &timerManager = owner.getPlaybackTimerManager();
        const auto &coordinator = owner.getInteractionCoordinator();
        const bool zDown = timerManager.isZKeyDown();
        const auto activePoint = coordinator.getActiveZoomPoint();
        const bool isZooming = zDown || activePoint != AppEnums::ActiveZoomPoint::None;

        if (isZooming && coordinator.getZoomPopupBounds()
                             .translated(-layout.getX(), -layout.getY())
                             .contains(currentX, 10))
            setVisible(false);
        else
            setVisible(true);
    }
}

void PlaybackCursorView::paint(juce::Graphics &g) {
    auto &audioPlayer = owner.getAudioPlayer();
    const double audioLength = audioPlayer.getWaveformManager().getThumbnail().getTotalLength();
    if (audioLength <= 0.0)
        return;

    const auto waveformBounds = getLocalBounds();
    const double drawPosition = audioPlayer.getCurrentPosition();
    const float x = CoordinateMapper::secondsToPixels(
        drawPosition, (float)waveformBounds.getWidth(), audioLength);

    const juce::Colour cursorColor = Config::Colors::playbackCursor;

    PlaybackCursorGlow::renderGlow(g, juce::roundToInt(x), 0, getHeight(), cursorColor);
}
