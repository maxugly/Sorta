#ifndef AUDIOFILER_FPSPRESENTER_H
#define AUDIOFILER_FPSPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include <JuceHeader.h>

class FpsView;
class ControlPanel;

/**
 * @file FpsPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for calculating and displaying UI performance metrics.
 */

class FpsView;
class ControlPanel;

/**
 * @class FpsPresenter
 * @brief Calculates real-time frame rates and pushes them to the FpsView.
 * 
 * @details Architecturally, this presenter monitors the PlaybackTimerManager 
 *          heartbeat to calculate visual performance metrics. It keeps the 
 *          FpsView (View) "dumb" by handling all time-based delta calculations 
 *          and averaging logic.
 * 
 * @see SessionState
 * @see ControlPanel
 * @see FpsView
 */
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
