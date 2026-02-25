#include "UI/Views/FpsView.h"
#include "Utils/Config.h"

FpsView::FpsView() {
    setInterceptsMouseClicks(false, false);
}

void FpsView::setFps(int fps, double frameTimeMs) {
    currentFpsText = juce::String(fps) + " FPS (" + juce::String(frameTimeMs, 1) + "ms)";
    repaint();
}

void FpsView::paint(juce::Graphics& g) {
    g.setColour(Config::Colors::fpsBackground);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Config::Layout::Fps::cornerRadius);
    g.setColour(Config::Colors::fpsText);
    g.setFont((float)Config::Layout::Text::mouseCursorSize);
    g.drawText(currentFpsText, getLocalBounds(), juce::Justification::centred, false);
}
