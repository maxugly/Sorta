

#ifndef AUDIOFILER_CONTROLSTATEPRESENTER_H
#define AUDIOFILER_CONTROLSTATEPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"

class ControlPanel;

class ControlStatePresenter final : public SessionState::Listener {
  public:
    explicit ControlStatePresenter(ControlPanel &ownerPanel);
    ~ControlStatePresenter() override;

    void refreshStates();

    void updateUIFromState();

    // SessionState::Listener overrides
    void fileChanged(const juce::String &filePath) override;
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;
    void cutInChanged(double value) override;
    void cutOutChanged(double value) override;

  private:
    void updateGeneralButtonStates(bool enabled);

    void updateCutModeControlStates(bool isCutModeActive, bool enabled);

    ControlPanel &owner;
};

#endif
