/**
 * @file BoundaryLockPresenter.cpp
 * @_deps/juce-build/tools/Makefile
 * @ingroup Logic
 * @brief Implementation of BoundaryLockPresenter.
 */

#include "Presenters/BoundaryLockPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

BoundaryLockPresenter::BoundaryLockPresenter(ControlPanel& cp, SessionState& ss)
    : owner(cp), sessionState(ss) {
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
        auto& lockBtn = cls->getLockButton();
        lockBtn.setToggleState(prefs.lengthLocked, juce::dontSendNotification);
        lockBtn.setButtonText(prefs.lengthLocked ? Config::Labels::lockLocked 
                                                 : Config::Labels::lockUnlocked);
    }
}
