#ifndef AUDIOFILER_SILENCEANALYSISWORKER_H
#define AUDIOFILER_SILENCEANALYSISWORKER_H

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class SilenceAnalysisWorker
 * @brief Handles the heavy lifting for detecting leading/trailing silence.
 *
 * SilenceDetector delegates to this helper so scanning logic (buffer allocation,
 * threshold comparisons, and loop updates) is isolated from UI concerns.
 */
class SilenceAnalysisWorker
{
public:
    /**
     * @brief Runs silence-in detection and updates loop start if a hit is found.
     * @param ownerPanel ControlPanel providing access to AudioPlayer and UI feedback.
     * @param threshold Normalized threshold (0-1) for detecting sound.
     */
    static void detectInSilence(ControlPanel& ownerPanel, float threshold);

    /**
     * @brief Runs silence-out detection and updates loop end if a hit is found.
     * @param ownerPanel ControlPanel providing access to AudioPlayer and UI feedback.
     * @param threshold Normalized threshold (0-1) for detecting sound.
     */
    static void detectOutSilence(ControlPanel& ownerPanel, float threshold);
};

#endif // AUDIOFILER_SILENCEANALYSISWORKER_H
