

#include "Presenters/CutButtonPresenter.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/InteractionCoordinator.h"
#include "Utils/Config.h"

CutButtonPresenter::CutButtonPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
    owner.getPlaybackTimerManager().addListener(this);
}

CutButtonPresenter::~CutButtonPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
}

void CutButtonPresenter::playbackTimerTick() {
    const auto currentMode = owner.getInteractionCoordinator().getPlacementMode();
    if (currentMode != lastPlacementMode) {
        updateColours();
        lastPlacementMode = currentMode;
    }
}

void CutButtonPresenter::animationUpdate(float) {
}

void CutButtonPresenter::updateColours() {
    const auto placementMode = owner.getInteractionCoordinator().getPlacementMode();

    if (owner.inStrip != nullptr)
        owner.inStrip->getMarkerButton().setColour(juce::TextButton::buttonColourId,
                                                   placementMode == AppEnums::PlacementMode::CutIn
                                                       ? Config::Colors::Button::cutPlacement
                                                       : Config::Colors::Button::cutActive);

    if (owner.outStrip != nullptr)
        owner.outStrip->getMarkerButton().setColour(juce::TextButton::buttonColourId,
                                                    placementMode == AppEnums::PlacementMode::CutOut
                                                        ? Config::Colors::Button::cutPlacement
                                                        : Config::Colors::Button::cutActive);
}
