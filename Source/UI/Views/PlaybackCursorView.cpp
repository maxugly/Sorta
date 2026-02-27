

#include "UI/Views/PlaybackCursorView.h"
#include "Core/AudioPlayer.h"
#include "Core/WaveformManager.h"
#include "UI/ControlPanel.h"
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
        cachedX = x;
        const int currentX = juce::roundToInt(x);

        if (currentX != lastCursorX) {
            const int glowWidth = 2;
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
    if (owner.getAudioPlayer().getWaveformManager().getThumbnail().getTotalLength() <= 0.0)
        return;

    const auto qColor = Config::Colors::quaternary;
    const juce::Colour cursorColor = juce::Colour((juce::uint8)(255 - qColor.getRed()),
                                                  (juce::uint8)(255 - qColor.getGreen()),
                                                  (juce::uint8)(255 - qColor.getBlue()));
    
    const int x = juce::roundToInt(cachedX);
    const float boxHeight = (float)Config::Layout::Glow::cutMarkerBoxHeight;
    const float boxWidth = Config::Layout::Glow::cutMarkerBoxWidth;
    const float halfBoxWidth = boxWidth / 2.0f;

    // Green Top/Bottom Squares
    g.setColour(juce::Colours::lime);
    g.fillRect((float)x - halfBoxWidth, 0.0f, boxWidth, boxHeight);
    g.fillRect((float)x - halfBoxWidth, (float)getHeight() - boxHeight, boxWidth, boxHeight);

    // Inverted Quaternary Center Line
    g.setColour(cursorColor);
    g.drawVerticalLine(x, boxHeight, (float)getHeight() - boxHeight);
}
