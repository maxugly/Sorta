#ifndef AUDIOFILER_MATRIXPRESENTER_H
#define AUDIOFILER_MATRIXPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "UI/Views/MatrixView.h"
#include "Core/SessionState.h"
#include "UI/InteractionCoordinator.h"
#include <juce_audio_basics/juce_audio_basics.h>

class ControlPanel;
class AudioPlayer;

class MatrixPresenter final : public PlaybackTimerManager::Listener,
                              public SessionState::Listener,
                              public InteractionCoordinator::Listener,
                              public juce::ChangeListener {
public:
    explicit MatrixPresenter(ControlPanel& cp);
    ~MatrixPresenter() override;

    void playbackTimerTick() override;
    void fullMatrixUpdate();

    // Event Hooks
    void fileChanged(const juce::String&) override { fullMatrixUpdate(); }
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override { fullMatrixUpdate(); }
    void volumeChanged(float) override { fullMatrixUpdate(); }
    void viewModeChanged(AppEnums::ViewMode) override { fullMatrixUpdate(); }
    void channelViewModeChanged(AppEnums::ChannelViewMode) override { fullMatrixUpdate(); }
    void placementModeChanged(AppEnums::PlacementMode) override { fullMatrixUpdate(); }
    void changeListenerCallback(juce::ChangeBroadcaster*) override { fullMatrixUpdate(); }

private:
    ControlPanel& owner;
    MatrixViewState cachedState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixPresenter)
};

#endif
