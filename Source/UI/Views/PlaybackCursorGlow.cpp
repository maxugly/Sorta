

#include "UI/Views/PlaybackCursorGlow.h"

void PlaybackCursorGlow::renderGlow(juce::Graphics &g, int x, int topY, int bottomY,
                                    juce::Colour baseColor) {
    const float glowWidth = Config::Layout::Glow::thickness;
    g.setColour(baseColor);
    g.fillRect((float)x - glowWidth, (float)topY, glowWidth * 2.0f, (float)(bottomY - topY));
}
