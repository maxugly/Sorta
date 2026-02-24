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

void OverlayView::paint(juce::Graphics &g) {
    juce::ignoreUnused(g);
}
