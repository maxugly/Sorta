

#include "Presenters/SilenceDetectionPresenter.h"

#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "Core/SilenceAnalysisWorker.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "Workers/SilenceDetector.h"

SilenceDetectionPresenter::SilenceDetectionPresenter(ControlPanel &ownerPanel,
                                                     SessionState &sessionStateIn,
                                                     AudioPlayer &audioPlayerIn)
    : owner(ownerPanel), sessionState(sessionStateIn), audioPlayer(audioPlayerIn),
      silenceWorker(*this, sessionStateIn, audioPlayerIn.getFormatManager()) {
    sessionState.addListener(this);
    owner.getPlaybackTimerManager().addListener(this);

    auto prefs = sessionState.getCutPrefs();
    lastAutoCutThresholdIn = prefs.autoCut.thresholdIn;
    lastAutoCutThresholdOut = prefs.autoCut.thresholdOut;
    lastAutoCutInActive = prefs.autoCut.inActive;
    lastAutoCutOutActive = prefs.autoCut.outActive;
}

SilenceDetectionPresenter::~SilenceDetectionPresenter() {
    owner.getPlaybackTimerManager().removeListener(this);
    sessionState.removeListener(this);
}

void SilenceDetectionPresenter::playbackTimerTick() {
}

void SilenceDetectionPresenter::animationUpdate(float breathingPulse) {
    const auto &autoCut = sessionState.getCutPrefs().autoCut;

    auto updateButton = [&](juce::TextButton &btn, bool isActive, bool isBusy) {
        if (isActive || isBusy) {
            btn.getProperties().set("isProcessing", true);
            btn.getProperties().set("pulseAlpha", breathingPulse);
            btn.repaint();
        } else {
            if (btn.getProperties().contains("isProcessing") &&
                (bool)btn.getProperties()["isProcessing"]) {
                btn.getProperties().set("isProcessing", false);
                btn.repaint();
            }
        }
    };

    updateButton(owner.getAutoCutInButton(), autoCut.inActive,
                 silenceWorker.isBusy() && silenceWorker.isDetectingIn());
    updateButton(owner.getAutoCutOutButton(), autoCut.outActive,
                 silenceWorker.isBusy() && !silenceWorker.isDetectingIn());
}

void SilenceDetectionPresenter::fileChanged(const juce::String &filePath) {
    if (filePath.isEmpty())
        return;

    const FileMetadata activeMetadata = sessionState.getMetadataForFile(filePath);
    if (!activeMetadata.isAnalyzed) {
        auto prefs = sessionState.getCutPrefs();
        if (prefs.autoCut.inActive)
            startSilenceAnalysis(prefs.autoCut.thresholdIn, true);

        if (prefs.autoCut.outActive)
            startSilenceAnalysis(prefs.autoCut.thresholdOut, false);
    }
}

void SilenceDetectionPresenter::cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
    const auto &autoCut = prefs.autoCut;

    // Check for significant change (> 0.01)
    const bool inThresholdChanged =
        std::abs(autoCut.thresholdIn - lastAutoCutThresholdIn) > 0.01001f;
    const bool outThresholdChanged =
        std::abs(autoCut.thresholdOut - lastAutoCutThresholdOut) > 0.01001f;

    const bool inActiveChanged = autoCut.inActive != lastAutoCutInActive;
    const bool outActiveChanged = autoCut.outActive != lastAutoCutOutActive;

    const bool shouldAnalyzeIn = (inThresholdChanged || inActiveChanged) && autoCut.inActive;
    const bool shouldAnalyzeOut = (outThresholdChanged || outActiveChanged) && autoCut.outActive;

    if (shouldAnalyzeIn)
        startSilenceAnalysis(autoCut.thresholdIn, true);

    if (shouldAnalyzeOut)
        startSilenceAnalysis(autoCut.thresholdOut, false);

    lastAutoCutThresholdIn = autoCut.thresholdIn;
    lastAutoCutThresholdOut = autoCut.thresholdOut;
    lastAutoCutInActive = autoCut.inActive;
    lastAutoCutOutActive = autoCut.outActive;
}

void SilenceDetectionPresenter::handleAutoCutInToggle(bool isActive) {
    sessionState.setAutoCutInActive(isActive);
}

void SilenceDetectionPresenter::handleAutoCutOutToggle(bool isActive) {
    sessionState.setAutoCutOutActive(isActive);
}

void SilenceDetectionPresenter::startSilenceAnalysis(float threshold, bool detectingIn) {
    if (silenceWorker.isBusy()) {
        // If it's busy with the same thing, ignore.
        if (silenceWorker.isDetectingIn() == detectingIn)
            return;

        // If it's busy with the OTHER thing, we might want to queue it or just wait.
        // For now, let's at least log it.
        return;
    }
    silenceWorker.startAnalysis(threshold, detectingIn, audioPlayer.getLoadedFile().getFullPathName());
}

void SilenceDetectionPresenter::logStatusMessage(const juce::String &message, bool isError) {
    const auto color = isError ? Config::Colors::statsErrorText : Config::Colors::statsText;
    owner.getPresenterCore().getStatsPresenter().setDisplayText(message, color);
}

bool SilenceDetectionPresenter::isCutModeActive() const {
    return sessionState.getCutPrefs().active;
}

bool SilenceDetectionPresenter::isAutoCutInActive() const {
    return sessionState.getCutPrefs().autoCut.inActive;
}

bool SilenceDetectionPresenter::isAutoCutOutActive() const {
    return sessionState.getCutPrefs().autoCut.outActive;
}

bool SilenceDetectionPresenter::hasLoadedAudio() const {
    return audioPlayer.getThumbnail().getTotalLength() > 0.0;
}
