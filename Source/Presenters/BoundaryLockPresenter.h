/**
 * @file BoundaryLockPresenter.h
 * @_deps/juce-build/tools/Makefile
 * @ingroup Logic
 * @brief Manages the visual and logical state of the cut length lock button.
 * @details This presenter synchronizes the toggle state of the lock button in the 
 *          CutLengthStrip with the SessionState's lengthLocked preference.
 *          It ensures that when the length is locked, moving one boundary
 *          appropriately shifts the other to maintain a constant duration.
 * 
 * @see CutLengthStrip
 * @see SessionState
 * @see ControlPanel
 */

#pragma once

#include "Core/SessionState.h"

class ControlPanel;

class BoundaryLockPresenter final : public SessionState::Listener {
public:
    /**
     * @brief Constructor for BoundaryLockPresenter.
     * @param owner Reference to the ControlPanel owning the UI components.
     * @param sessionState Reference to the SessionState containing the data model.
     */
    BoundaryLockPresenter(ControlPanel& owner, SessionState& sessionState);
    
    /** @brief Destructor for BoundaryLockPresenter. */
    ~BoundaryLockPresenter() override;

    /** @brief SessionState::Listener override to sync UI when preferences change. */
    void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) override;

private:
    ControlPanel& owner;
    SessionState& sessionState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoundaryLockPresenter)
};
