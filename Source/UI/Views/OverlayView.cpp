#include "UI/Views/OverlayView.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include <vector>

OverlayView::OverlayView(ControlPanel &ownerIn) : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);
}

OverlayView::~OverlayView() = default;

void OverlayView::paint(juce::Graphics &g) {
    juce::ignoreUnused(g);
}
