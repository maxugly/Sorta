#ifndef AUDIOFILER_SILENCEDETECTIONPRESENTER_H
#define AUDIOFILER_SILENCEDETECTIONPRESENTER_H

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class SilenceDetectionPresenter
 * @brief Handles auto-cut toggle behaviour and detection triggers for the SilenceDetector.
 *
 * By moving the auto-cut wiring here, ControlPanel simply forwards button events while
 * this presenter coordinates SilenceDetector state, component refreshes, and optional
 * immediate detection when audio is loaded.
 */
class SilenceDetectionPresenter final
{
public:
    /**
     * @brief Binds the presenter to a specific ControlPanel instance.
     * @param ownerPanel ControlPanel whose SilenceDetector is coordinated.
     */
    explicit SilenceDetectionPresenter(ControlPanel& ownerPanel);

    /**
     * @brief Handles toggling the auto-cut-in mode.
     * @param isActive True when auto-cut-in should be active.
     */
    void handleAutoCutInToggle(bool isActive);

    /**
     * @brief Handles toggling the auto-cut-out mode.
     * @param isActive True when auto-cut-out should be active.
     */
    void handleAutoCutOutToggle(bool isActive);

private:
    bool hasLoadedAudio() const;

    ControlPanel& owner;
};

#endif // AUDIOFILER_SILENCEDETECTIONPRESENTER_H
