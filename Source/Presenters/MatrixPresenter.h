#ifndef AUDIOFILER_MATRIXPRESENTER_H
#define AUDIOFILER_MATRIXPRESENTER_H

#include "Presenters/PlaybackTimerManager.h"
#include "UI/Views/MatrixView.h"
#include "Core/SessionState.h"
#include "UI/InteractionCoordinator.h"
#include <juce_audio_basics/juce_audio_basics.h>

class ControlPanel;
class AudioPlayer;

/**
 * @file MatrixPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for orchestrating the visual state of the status LED matrix.
 */

class ControlPanel;
class AudioPlayer;

/**
 * @class MatrixPresenter
 * @brief Manages the logic for the auxiliary LED-style status grid.
 * 
 * @details Architecturally, this presenter acts as a bridge between the 
 *          various system states (Model/SessionState) and the MatrixView. 
 *          It aggregates information from playback, volume, and view modes 
 *          to determine which LEDs should be illuminated.
 * 
 * @see SessionState
 * @see ControlPanel
 * @see MatrixView
 */
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
