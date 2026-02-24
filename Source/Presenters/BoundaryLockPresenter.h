#pragma once

#include "Core/SessionState.h"

class ControlPanel;

class BoundaryLockPresenter final : public SessionState::Listener {
public:
    explicit BoundaryLockPresenter(ControlPanel& owner);
    ~BoundaryLockPresenter() override;

    void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) override;

private:
    ControlPanel& owner;
    SessionState& sessionState;
};
