
#ifndef AUDIOFILER_MATRIXPRESENTER_H
#define AUDIOFILER_MATRIXPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "UI/Views/MatrixView.h"

class ControlPanel;
class AudioPlayer;
class SessionState;
class InteractionCoordinator;

class MatrixPresenter final : public PlaybackTimerManager::Listener {
  public:
    explicit MatrixPresenter(ControlPanel& cp);
    ~MatrixPresenter() override;

    void playbackTimerTick() override;
    void animationUpdate(float breathingPulse) override;

  private:
    ControlPanel& owner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixPresenter)
};

#endif
