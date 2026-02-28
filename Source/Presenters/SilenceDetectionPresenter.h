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

/**
 * @file SilenceDetectionPresenter.h
 * @ingroup Logic
 * @brief Presenter responsible for orchestrating background silence analysis and UI status reporting.
 * 
 * @details Architecturally, SilenceDetectionPresenter acts as the "Presenter Glue" that 
 *          bridges the gap between the asynchronous `SilenceAnalysisWorker` and the 
 *          visual status indicators in the `ControlPanel`. It implements the MVP 
 *          Law by monitoring user preference changes in `SessionState` and 
 *          dispatching analysis tasks to the background thread.
 * 
 *          Key responsibilities:
 *          - **Task Dispatching**: Triggers `SilenceAnalysisWorker` passes when 
 *            new files are loaded or when the user toggles "Auto-Cut" modes.
 *          - **Status Marshalling**: Implements `SilenceWorkerClient` to receive 
 *            log messages from the background thread and safely push them to 
 *            the UI's status bar.
 *          - **Change Monitoring**: Observes `SessionState` and `PlaybackTimerManager` 
 *            to ensure the analysis state remains synchronized with the active audio file.
 * 
 * @see SilenceAnalysisWorker
 * @see SilenceWorkerClient
 * @see SessionState
 * @see ControlPanel
 */
class SilenceDetectionPresenter final : public SilenceWorkerClient,
                                         public SessionState::Listener,
                                         public PlaybackTimerManager::Listener {
  public:
    /**
     * @brief Constructs the presenter and initializes the background worker.
     * @param ownerPanel Reference to the parent View container.
     * @param sessionStateIn Reference to the central state hub.
     * @param audioPlayerIn Reference to the audio engine (used for format management).
     */
    explicit SilenceDetectionPresenter(ControlPanel &ownerPanel,
                                       SessionState &sessionStateIn,
                                       AudioPlayer &audioPlayerIn);

    /**
     * @brief Ensures the background worker is safely terminated before destruction.
     */
    ~SilenceDetectionPresenter() override;

    /** 
     * @brief Periodic callback from the playback timer.
     * @details Used for high-frequency polling of worker status or UI animation updates.
     */
    void playbackTimerTick() override;

    /** 
     * @brief Triggers automatic re-analysis when the active audio file path changes.
     * @param filePath Absolute path to the new asset.
     */
    void fileChanged(const juce::String &filePath) override;

    /** 
     * @brief Reacts to threshold adjustments by triggering a background analysis pass.
     * @param prefs The updated user preferences from state.
     */
    void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) override;

    /** 
     * @brief Updates the SessionState toggle for automatic 'In' point detection.
     * @param isActive True to enable the auto-detection logic.
     */
    void handleAutoCutInToggle(bool isActive);

    /** 
     * @brief Updates the SessionState toggle for automatic 'Out' point detection.
     * @param isActive True to enable the auto-detection logic.
     */
    void handleAutoCutOutToggle(bool isActive);

    /** 
     * @brief Dispatches a specific analysis pass to the background thread.
     * @param threshold The dB level to use for silence detection.
     * @param detectingIn True to search for start-of-audio, false for end-of-audio.
     * @note This method handles the "Air Gap" by ensuring the worker is not already busy.
     */
    void startSilenceAnalysis(float threshold, bool detectingIn);

    /** 
     * @brief Queries the activity state of the background worker.
     * @return True if a scan is currently in progress.
     */
    bool isAnalyzing() const {
        return silenceWorker.isBusy();
    }

    /** @brief Specific query for 'In' point analysis status. */
    bool isAnalyzingIn() const { return silenceWorker.isBusy() && silenceWorker.isDetectingIn(); }
    
    /** @brief Specific query for 'Out' point analysis status. */
    bool isAnalyzingOut() const { return silenceWorker.isBusy() && !silenceWorker.isDetectingIn(); }

    /** 
     * @brief Safe entry point for background threads to push messages to the UI.
     * @param message Human-readable status or error string.
     * @param isError True if the message should be rendered with error styling.
     */
    void logStatusMessage(const juce::String &message, bool isError = false) override;

    /** @brief Queries SessionState for the current Auto-Cut-In preference. */
    bool isAutoCutInActive() const override;

    /** @brief Queries SessionState for the current Auto-Cut-Out preference. */
    bool isAutoCutOutActive() const override;

  private:
    ControlPanel &owner;                      /**< Reference to the host View. */
    SessionState &sessionState;                /**< The central Model. */
    AudioPlayer &audioPlayer;                 /**< The primary Audio Engine. */
    SilenceAnalysisWorker silenceWorker;      /**< The background-threaded processing entity. */

    float lastAutoCutThresholdIn{-1.0f};      /**< Cache to detect meaningful threshold changes. */
    float lastAutoCutThresholdOut{-1.0f};     /**< Cache to detect meaningful threshold changes. */
    bool lastAutoCutInActive{false};         /**< Cache to detect toggle changes. */
    bool lastAutoCutOutActive{false};        /**< Cache to detect toggle changes. */
};

#endif
