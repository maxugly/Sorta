

#ifndef AUDIOFILER_SILENCEDETECTIONPRESENTER_H
#define AUDIOFILER_SILENCEDETECTIONPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/SessionState.h"
#include "Core/SilenceAnalysisWorker.h"
#include "Presenters/PlaybackTimerManager.h"
#include "Workers/SilenceWorkerClient.h"

class ControlPanel;

class SilenceDetectionPresenter final : public SilenceWorkerClient,
                                         public SessionState::Listener,
                                         public PlaybackTimerManager::Listener {
  public:
    explicit SilenceDetectionPresenter(ControlPanel &ownerPanel,
                                       SessionState &sessionStateIn,
                                       AudioPlayer &audioPlayerIn);
    ~SilenceDetectionPresenter() override;

    /** @brief Called when the playback timer ticks at 60Hz. */
    void playbackTimerTick() override;

    /** @brief Updates the UI animation state based on the master pulse. */
    void animationUpdate(float breathingPulse) override;

    /** @brief Triggered when the current file in SessionState changes. */
    void fileChanged(const juce::String &filePath) override;

    /** @brief Triggered when cut preferences (thresholds, active state) change. */
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

    /** @brief Toggles the auto-cut-in feature in SessionState. */
    void handleAutoCutInToggle(bool isActive);

    /** @brief Toggles the auto-cut-out feature in SessionState. */
    void handleAutoCutOutToggle(bool isActive);

    /** @brief Manually triggers silence analysis with the specified threshold. */
    void startSilenceAnalysis(float threshold, bool detectingIn);

    /** @brief Returns true if a silence analysis task is currently running. */
    bool isAnalyzing() const {
        return silenceWorker.isBusy();
    }

    bool isAnalyzingIn() const { return silenceWorker.isBusy() && silenceWorker.isDetectingIn(); }
    bool isAnalyzingOut() const { return silenceWorker.isBusy() && !silenceWorker.isDetectingIn(); }

    /** @brief Logs a status message to the UI (via ControlPanel). */
    void logStatusMessage(const juce::String &message, bool isError = false) override;

    /** @brief Returns true if cut mode is currently active. */
    bool isCutModeActive() const override;

    /** @brief Returns true if auto-cut-in is active. */
    bool isAutoCutInActive() const override;

    /** @brief Returns true if auto-cut-out is active. */
    bool isAutoCutOutActive() const override;

  private:
    bool hasLoadedAudio() const;

    ControlPanel &owner;
    SessionState &sessionState;
    AudioPlayer &audioPlayer;
    SilenceAnalysisWorker silenceWorker;

    float lastAutoCutThresholdIn{-1.0f};
    float lastAutoCutThresholdOut{-1.0f};
    bool lastAutoCutInActive{false};
    bool lastAutoCutOutActive{false};
};

#endif
