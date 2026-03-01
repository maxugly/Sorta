

#include "Presenters/CutButtonPresenter.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/InteractionCoordinator.h"
#include "Utils/Config.h"

CutButtonPresenter::CutButtonPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
    owner.getInteractionCoordinator().addListener(this);
}

CutButtonPresenter::~CutButtonPresenter() {
    owner.getInteractionCoordinator().removeListener(this);
}

void CutButtonPresenter::placementModeChanged(AppEnums::PlacementMode newMode) {
    juce::ignoreUnused(newMode);
    updateColours();
    owner.repaint();
}

void CutButtonPresenter::updateColours() {
    const auto placementMode = owner.getInteractionCoordinator().getPlacementMode();

    if (owner.inStrip != nullptr) {
        if (placementMode == AppEnums::PlacementMode::CutIn) {
            owner.inStrip->getMarkerButton().setColour(juce::TextButton::buttonColourId, Config::Colors::Button::cutPlacement);
        } else {
            owner.inStrip->getMarkerButton().removeColour(juce::TextButton::buttonColourId);
        }
    }

    if (owner.outStrip != nullptr) {
        if (placementMode == AppEnums::PlacementMode::CutOut) {
            owner.outStrip->getMarkerButton().setColour(juce::TextButton::buttonColourId, Config::Colors::Button::cutPlacement);
        } else {
            owner.outStrip->getMarkerButton().removeColour(juce::TextButton::buttonColourId);
        }
    }
}
