

#ifndef AUDIOFILER_CUTPRESENTER_H
#define AUDIOFILER_CUTPRESENTER_H

#include "Core/SessionState.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"

class CutLayerView;

class ControlPanel;

class CutPresenter : public SessionState::Listener {
  public:
    CutPresenter(ControlPanel &controlPanel, SessionState &sessionState,
                 CutLayerView &cutLayerView);

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

  private:
    void refreshMarkersVisibility();

    SessionState &sessionState;
    CutLayerView &cutLayerView;
    std::unique_ptr<MarkerMouseHandler> markerMouseHandler;
    std::unique_ptr<WaveformMouseHandler> waveformMouseHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutPresenter)
};

#endif
