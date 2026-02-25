#include "Presenters/FpsPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/FpsView.h"

FpsPresenter::FpsPresenter(ControlPanel& ownerPanel, FpsView& view)
    : owner(ownerPanel), fpsView(view) {
    owner.getPlaybackTimerManager().addListener(this);
    lastTimeMs = juce::Time::getMillisecondCounterHiRes();
    lastTickTime = lastTimeMs;
}

FpsPresenter::~FpsPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void FpsPresenter::playbackTimerTick() {
    frameCount++;
    const double currentTime = juce::Time::getMillisecondCounterHiRes();
    
    if (lastTickTime > 0.0) {
        accumulatedFrameTime += (currentTime - lastTickTime);
    }
    lastTickTime = currentTime;

    if (currentTime >= lastTimeMs + 1000.0) {
        double avgFrameTime = frameCount > 0 ? (accumulatedFrameTime / frameCount) : 0.0;
        fpsView.setFps(frameCount, avgFrameTime);
        
        frameCount = 0;
        accumulatedFrameTime = 0.0;
        lastTimeMs = currentTime;
    }
}
