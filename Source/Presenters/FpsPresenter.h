#ifndef AUDIOFILER_FPSPRESENTER_H
#define AUDIOFILER_FPSPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include <JuceHeader.h>

class FpsView;
class ControlPanel;

class FpsPresenter final : public PlaybackTimerManager::Listener {
public:
    FpsPresenter(ControlPanel& ownerPanel, FpsView& view);
    ~FpsPresenter() override;
    void playbackTimerTick() override;
private:
    ControlPanel& owner;
    FpsView& fpsView;
    double lastTimeMs{0.0};
    double lastTickTime{0.0};
    double accumulatedFrameTime{0.0};
    int frameCount{0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FpsPresenter)
};
#endif
