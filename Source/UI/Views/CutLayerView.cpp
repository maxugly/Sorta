#include "UI/Views/CutLayerView.h"

#include "UI/ControlPanel.h"
#include "UI/Views/CutFadeView.h"
#include "UI/Views/CutMarkersView.h"
#include "UI/Views/CutThresholdsView.h"

CutLayerView::CutLayerView(ControlPanel &ownerIn)
    : owner(ownerIn) {
    setInterceptsMouseClicks(false, false);
    setOpaque(false);

    fadeView = std::make_unique<CutFadeView>();
    thresholdsView = std::make_unique<CutThresholdsView>();
    markersView = std::make_unique<CutMarkersView>();

    addAndMakeVisible(*fadeView);
    addAndMakeVisible(*thresholdsView);
    addAndMakeVisible(*markersView);
}

CutLayerView::~CutLayerView() = default;

void CutLayerView::updateState(const CutLayerState& newState) {
    fadeView->updateState(newState);
    thresholdsView->updateState(newState);
    markersView->updateState(newState);

    state = newState;
}

void CutLayerView::resized() {
    auto bounds = getLocalBounds();
    fadeView->setBounds(bounds);
    thresholdsView->setBounds(bounds);
    markersView->setBounds(bounds);
}
