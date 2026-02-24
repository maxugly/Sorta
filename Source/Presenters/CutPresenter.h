

#ifndef AUDIOFILER_CUTPRESENTER_H
#define AUDIOFILER_CUTPRESENTER_H

#include "Core/SessionState.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "Presenters/PlaybackTimerManager.h"

class CutLayerView;
struct CutLayerState;
class ControlPanel;
class InteractionCoordinator;

class CutPresenter : public SessionState::Listener,
                     public PlaybackTimerManager::Listener {
  public:
    CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                 CutLayerView &cutLayerViewIn,
                 InteractionCoordinator &interactionCoordinatorIn,
                 PlaybackTimerManager &playbackTimerManagerIn);


    ~CutPresenter() override;

    MarkerMouseHandler &getMarkerMouseHandler() {
        return *markerMouseHandler;
    }

    const MarkerMouseHandler &getMarkerMouseHandler() const {
        return *markerMouseHandler;
    }

    WaveformMouseHandler &getWaveformMouseHandler() {
        return *waveformMouseHandler;
    }

    const WaveformMouseHandler &getWaveformMouseHandler() const {
        return *waveformMouseHandler;
    }

    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;
    void cutInChanged(double value) override { juce::ignoreUnused(value); pushStateToView(); }
    void cutOutChanged(double value) override { juce::ignoreUnused(value); pushStateToView(); }
    void fileChanged(const juce::String &filePath) override { juce::ignoreUnused(filePath); pushStateToView(); }

    // PlaybackTimerManager::Listener overrides
    void playbackTimerTick() override {}

  private:
    void refreshMarkersVisibility();
    void pushStateToView();
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
