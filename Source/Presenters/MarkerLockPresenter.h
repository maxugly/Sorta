/**
 * @file MarkerLockPresenter.h
 * @_deps/juce-build/tools/Makefile
 * @ingroup Logic
 * @brief Manages the visual and logical state of the In and Out point lock buttons.
 * @details This presenter synchronizes the toggle state of the lock buttons in the 
 *          MarkerStrips (In and Out) with the SessionState's inLocked and outLocked preferences.
 *          It ensures that when a marker is locked, its position cannot be modified.
 * 
 * @see MarkerStrip
 * @see SessionState
 */

#pragma once

#include "Core/SessionState.h"

class ControlPanel;

class MarkerLockPresenter final : public SessionState::Listener {
public:
    /**
     * @brief Constructor for MarkerLockPresenter.
     * @param owner Reference to the ControlPanel owning the UI components.
     * @param sessionState Reference to the SessionState containing the data model.
     */
    MarkerLockPresenter(ControlPanel& owner, SessionState& sessionState);
    
    /** @brief Destructor for MarkerLockPresenter. */
    ~MarkerLockPresenter() override;

    /** @brief SessionState::Listener override to sync UI when preferences change. */
    void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) override;

private:
    ControlPanel& owner;
    SessionState& sessionState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerLockPresenter)
};
