
#ifndef AUDIOFILER_MATRIXPRESENTER_H
#define AUDIOFILER_MATRIXPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "UI/Views/MatrixView.h"

class ControlPanel;
class AudioPlayer;
class SessionState;
class SilenceDetector;
class InteractionCoordinator;

class MatrixPresenter final : public PlaybackTimerManager::Listener {
  public:
    MatrixPresenter(ControlPanel& cp, AudioPlayer& player, SessionState& state,
                    SilenceDetector& detector, InteractionCoordinator& coordinator,
                    MatrixView& view);
    ~MatrixPresenter() override;

    void playbackTimerTick() override;
    void animationUpdate(float breathingPulse) override;

  private:
    ControlPanel& owner;
    AudioPlayer& audioPlayer;
    SessionState& sessionState;
    SilenceDetector& silenceDetector;
    InteractionCoordinator& interactionCoordinator;
    MatrixView& matrixView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixPresenter)
};

#endif
