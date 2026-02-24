#include "Presenters/BoundaryLockPresenter.h"
#include "UI/ControlPanel.h"

BoundaryLockPresenter::BoundaryLockPresenter(ControlPanel& cp)
    : owner(cp), sessionState(cp.getSessionState()) {
    sessionState.addListener(this);

    if (auto* cls = owner.getCutLengthStrip()) {
        cls->getLockButton().onClick = [this]() {
            sessionState.setLengthLocked(owner.getCutLengthStrip()->getLockButton().getToggleState());
        };
    }
}

BoundaryLockPresenter::~BoundaryLockPresenter() {
    sessionState.removeListener(this);
}

void BoundaryLockPresenter::cutPreferenceChanged(const MainDomain::CutPreferences& prefs) {
    if (auto* cls = owner.getCutLengthStrip()) {
        cls->getLockButton().setToggleState(prefs.lengthLocked, juce::dontSendNotification);
    }
}
