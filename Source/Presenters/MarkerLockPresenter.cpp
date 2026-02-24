/**
 * @file MarkerLockPresenter.cpp
 * @_deps/juce-build/tools/Makefile
 * @ingroup Logic
 * @brief Implementation of MarkerLockPresenter.
 */

#include "Presenters/MarkerLockPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

MarkerLockPresenter::MarkerLockPresenter(ControlPanel& cp, SessionState& ss)
    : owner(cp), sessionState(ss) {
    sessionState.addListener(this);

    if (auto* inStr = owner.getInStrip()) {
        inStr->getLockButton().onClick = [this]() {
            sessionState.setInLocked(owner.getInStrip()->getLockButton().getToggleState());
        };
    }

    if (auto* outStr = owner.getOutStrip()) {
        outStr->getLockButton().onClick = [this]() {
            sessionState.setOutLocked(owner.getOutStrip()->getLockButton().getToggleState());
        };
    }
}

MarkerLockPresenter::~MarkerLockPresenter() {
    sessionState.removeListener(this);
}

void MarkerLockPresenter::cutPreferenceChanged(const MainDomain::CutPreferences& prefs) {
    if (auto* inStr = owner.getInStrip()) {
        auto& lockBtn = inStr->getLockButton();
        lockBtn.setToggleState(prefs.inLocked, juce::dontSendNotification);
        lockBtn.setButtonText(prefs.inLocked ? Config::Labels::lockLocked 
                                             : Config::Labels::lockUnlocked);
    }

    if (auto* outStr = owner.getOutStrip()) {
        auto& lockBtn = outStr->getLockButton();
        lockBtn.setToggleState(prefs.outLocked, juce::dontSendNotification);
        lockBtn.setButtonText(prefs.outLocked ? Config::Labels::lockLocked 
                                              : Config::Labels::lockUnlocked);
    }
}
