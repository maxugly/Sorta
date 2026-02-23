#pragma once

#include "Core/SessionState.h"
#include "UI/Views/VolumeView.h"

/**
 * @file VolumePresenter.h
 * @ingroup Logic
 * @brief Presenter for the master volume control.
 */
class VolumePresenter final : public SessionState::Listener {
  public:
    VolumePresenter(VolumeView &v, SessionState &s);
    ~VolumePresenter() override;

    void volumeChanged(float newVolume) override;

  private:
    VolumeView &view;
    SessionState &sessionState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumePresenter)
};
