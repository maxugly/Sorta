#include "Presenters/HintPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

HintPresenter::HintPresenter(ControlPanel& ownerPanel, HintView& view) 
    : owner(ownerPanel), hintView(view) {
    // The 'true' flag attaches this listener to ALL child components natively!
    owner.addMouseListener(this, true); 
}

HintPresenter::~HintPresenter() {
    owner.removeMouseListener(this);
}

void HintPresenter::mouseEnter(const juce::MouseEvent& event) {
    if (event.eventComponent == nullptr) return;

    juce::String hintText = event.eventComponent->getProperties().getWithDefault("tooltip", "").toString();
    
    if (hintText.isEmpty()) hintText = event.eventComponent->getName();
    if (hintText.isEmpty()) {
        if (auto* btn = dynamic_cast<juce::Button*>(event.eventComponent))
            hintText = btn->getButtonText();
    }
    
    // Fallback if no valid text was found
    if (hintText.isEmpty() || hintText.length() <= 2) 
        hintText = Config::Labels::defaultHint;
    
    // Strip out the bracket hotkeys (e.g. "[A]utoplay" becomes "utoplay" so we strip brackets entirely)
    hintView.setHint(hintText.removeCharacters("[]")); 
}

void HintPresenter::mouseExit(const juce::MouseEvent&) {
    hintView.setHint(Config::Labels::defaultHint);
}
