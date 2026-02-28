#ifndef AUDIOFILER_CUTPRESENTER_H
#define AUDIOFILER_CUTPRESENTER_H

#include "Core/SessionState.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "Presenters/PlaybackTimerManager.h"

/**
 * @file CutPresenter.h
 * @ingroup UI
 * @brief Coordinator and presenter for the visual representation of audio cuts.
 */

class CutLayerView;
struct CutLayerState;
class ControlPanel;
class InteractionCoordinator;

/**
 * @class CutPresenter
 * @brief Orchestrates the visual state and interaction for audio cut boundaries.
 * 
 * @details In the Model-View-Presenter (MVP) architecture, CutPresenter acts as a high-level 
 *          coordinator that bridges the SessionState (Model) and the CutLayerView (View). 
 *          It owns and delegates granular mouse interactions to specialized handlers 
 *          (MarkerMouseHandler and WaveformMouseHandler), effectively centralizing the 
 *          logic for how cut markers and waveform highlights should behave. By pushing 
 *          calculated states to the CutLayerView, it ensures the view remains a pure 
 *          rendering component while the presenter handles the "glue" logic of 
 *          synchronizing playback, markers, and user input.
 * 
 * @see SessionState, CutLayerView, MarkerMouseHandler, WaveformMouseHandler, InteractionCoordinator
 */
class CutPresenter : public SessionState::Listener,
                     public PlaybackTimerManager::Listener {
  public:
    /**
     * @brief Constructs a new CutPresenter.
     * @param controlPanel The parent control panel.
     * @param sessionStateIn The application session state.
     * @param cutLayerViewIn The view responsible for rendering cut layers.
     * @param interactionCoordinatorIn Coordinates interactions across multiple presenters.
     * @param playbackTimerManagerIn Manages playback timing for synchronizing visuals.
     */
    CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                 CutLayerView &cutLayerViewIn,
                 InteractionCoordinator &interactionCoordinatorIn,
                 PlaybackTimerManager &playbackTimerManagerIn);

    /** @brief Destructor. */
    ~CutPresenter() override;

    /** @return Reference to the marker mouse interaction handler. */
    MarkerMouseHandler &getMarkerMouseHandler() {
        return *markerMouseHandler;
    }

    /** @return Const reference to the marker mouse interaction handler. */
    const MarkerMouseHandler &getMarkerMouseHandler() const {
        return *markerMouseHandler;
    }

    /** @return Reference to the waveform mouse interaction handler. */
    WaveformMouseHandler &getWaveformMouseHandler() {
        return *waveformMouseHandler;
    }

    /** @return Const reference to the waveform mouse interaction handler. */
    const WaveformMouseHandler &getWaveformMouseHandler() const {
        return *waveformMouseHandler;
    }

    /** @brief Triggered when cut preferences change in the SessionState. */
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

    /** @brief Triggered when the 'In' point changes in the SessionState. */
    void cutInChanged(double value) override { juce::ignoreUnused(value); pushStateToView(); }

    /** @brief Triggered when the 'Out' point changes in the SessionState. */
    void cutOutChanged(double value) override { juce::ignoreUnused(value); pushStateToView(); }

    /** @brief Triggered when the active audio file changes. */
    void fileChanged(const juce::String &filePath) override { juce::ignoreUnused(filePath); pushStateToView(); }

    /** 
     * @brief Triggered by the PlaybackTimerManager on every tick. 
     * @details Updates the view's playback playhead position and triggers any 
     *          necessary visual animations.
     */
    void playbackTimerTick() override;

  private:
    /** @brief Updates the visibility of markers based on current state (e.g., file loaded). */
    void refreshMarkersVisibility();

    /** @brief Synchronizes the current SessionState with the CutLayerView. */
    void pushStateToView();

    /** @brief Updates the animation parameters for the view based on state changes. */
    void updateAnimationState(CutLayerState& state);

  private:
    SessionState &sessionState;
    CutLayerView &cutLayerView;
    InteractionCoordinator &interactionCoordinator;

    PlaybackTimerManager &playbackTimerManager;

    std::unique_ptr<MarkerMouseHandler> markerMouseHandler;
    std::unique_ptr<WaveformMouseHandler> waveformMouseHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutPresenter)
};

#endif
