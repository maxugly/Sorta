#include "PlaybackOverlay.h"
#include "ControlPanel.h"

PlaybackOverlay::PlaybackOverlay(ControlPanel& ownerPanel)
    : owner(ownerPanel)
{
}

void PlaybackOverlay::paint(juce::Graphics& g)
{
    // Translate the coordinate system so that (0,0) in logic (ControlPanel coordinates)
    // maps to (0,0) in this component, relative to its position.
    // Since PlaybackOverlay is positioned at (waveformBounds.x, waveformBounds.y),
    // and WaveformRenderer draws at (waveformBounds.x, waveformBounds.y),
    // we need to translate by -x, -y.

    g.setOrigin(-getX(), -getY());

    owner.renderOverlays(g);
}
