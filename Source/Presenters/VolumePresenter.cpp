#include "Presenters/VolumePresenter.h"

VolumePresenter::VolumePresenter(VolumeView &v, SessionState &s)
    : view(v), sessionState(s) {
    sessionState.addListener(this);

    view.getSlider().onValueChange = [this] {
        sessionState.setVolume((float)view.getSlider().getValue());
    };

    view.getSlider().setValue(sessionState.getVolume(), juce::dontSendNotification);
}

VolumePresenter::~VolumePresenter() {
    sessionState.removeListener(this);
}

void VolumePresenter::volumeChanged(float newVolume) {
    view.getSlider().setValue(newVolume, juce::dontSendNotification);
}
