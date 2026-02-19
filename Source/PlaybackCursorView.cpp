#include "PlaybackCursorView.h"
#include "ControlPanel.h"
#include "PlaybackCursorGlow.h"
#include "Config.h"
#include "CoordinateMapper.h"

PlaybackCursorView::PlaybackCursorView(ControlPanel& ownerPanel)
    : owner(ownerPanel)
{
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

void PlaybackCursorView::paint(juce::Graphics& g)
{
    g.setColour(Config::Colors::playbackText);
    g.drawVerticalLine(1, 0.0f, (float)getHeight());
}
