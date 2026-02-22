

#include "Presenters/CutPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/CutLayerView.h"

CutPresenter::CutPresenter(ControlPanel &controlPanel, SessionState &sessionStateIn,
                           CutLayerView &cutLayerViewIn)
    : sessionState(sessionStateIn), cutLayerView(cutLayerViewIn) {
    markerMouseHandler = std::make_unique<MarkerMouseHandler>(controlPanel);
    waveformMouseHandler = std::make_unique<WaveformMouseHandler>(controlPanel);
    sessionState.addListener(this);

    refreshMarkersVisibility();
}

CutPresenter::~CutPresenter() {
    sessionState.removeListener(this);
}

void CutPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &) {
    refreshMarkersVisibility();
    cutLayerView.repaint();
}

void CutPresenter::refreshMarkersVisibility() {
    const auto prefs = sessionState.getCutPrefs();
    cutLayerView.setMarkersVisible(prefs.active);
}
