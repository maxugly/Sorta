#ifndef AUDIOFILER_CONTROLPANEL_H
#define AUDIOFILER_CONTROLPANEL_H

#include "Presenters/PlaybackTimerManager.h"
#include "Presenters/PresenterCore.h"
#include "UI/InteractionCoordinator.h"
#include "UI/Views/MatrixView.h"
#include "UI/Views/HintView.h"
#include "UI/Views/FpsView.h"

class FocusManager;
#include "Core/AppEnums.h"
#include "Core/AudioPlayer.h"
#include "Core/SessionState.h"
#include "UI/Components/MarkerStrip.h"
#include "UI/Components/TransportButton.h"
#include "UI/Components/TransportStrip.h"
#include "UI/Components/CutLengthStrip.h"
#include "UI/Views/PlaybackTimeView.h"
#include "UI/Views/TopBarView.h"
#include "UI/Handlers/MarkerMouseHandler.h"
#include "UI/Handlers/WaveformMouseHandler.h"
#include "UI/LookAndFeel/ModernLookAndFeel.h"
#include "Utils/Config.h"
#include "Workers/SilenceWorkerClient.h"

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <memory>
#include <tuple>

class MainComponent;
class LayoutManager;
class WaveformCanvasView;
class CutLayerView;
class CutPresenter;
class StatsPresenter;
class ControlStatePresenter;
class SilenceDetectionPresenter;
class RepeatButtonPresenter;
class BoundaryLogicPresenter;
class ControlButtonsPresenter;
class CutResetPresenter;
class CutButtonPresenter;
class PlaybackTextPresenter;
class PlaybackCursorView;
class ZoomView;
class PlaybackRepeatController;
class TransportStrip;
class MarkerStrip;
class OverlayView;

/**
 * @file ControlPanel.h
 * @ingroup UI
 * @brief The visual container for UI controls and waveform visualization.
 * @details This component no longer owns its logic managers; they are injected by MainComponent.
 */
class ControlPanel final : public juce::Component {
  public:
    struct LayoutCache {
        juce::Rectangle<int> waveformBounds;
        juce::Rectangle<int> contentAreaBounds;
    };

    explicit ControlPanel(MainComponent &owner, SessionState &sessionStateIn);
    ~ControlPanel() override;

    void injectLogic(InteractionCoordinator& ic, PlaybackTimerManager& ptm, PresenterCore& pc, FocusManager& fm);
    void setInteractionCoordinator(InteractionCoordinator& ic) { interactionCoordinator = &ic; }
    void setPlaybackTimerManager(PlaybackTimerManager& ptm) { playbackTimerManager = &ptm; }
    void setPresenterCore(PresenterCore& pc) { presenterCore = &pc; }
    void setFocusManager(FocusManager& fm) { focusManager = &fm; }

    void paint(juce::Graphics &g) override;
    void resized() override;
    juce::MouseCursor getMouseCursor() override;

    juce::TextButton &getAutoCutInButton() { return inStrip->getAutoCutButton(); }
    juce::TextButton &getAutoCutOutButton() { return outStrip->getAutoCutButton(); }

    juce::Rectangle<int> getWaveformBounds() const { return layoutCache.waveformBounds; }

    AudioPlayer &getAudioPlayer();
    AudioPlayer &getAudioPlayer() const;
    SessionState &getSessionState() { return sessionState; }
    const SessionState &getSessionState() const { return sessionState; }

    InteractionCoordinator &getInteractionCoordinator() { return *interactionCoordinator; }

    AppEnums::ChannelViewMode getChannelViewMode() const { return sessionState.getChannelViewMode(); }

    const MarkerMouseHandler &getMarkerMouseHandler() const;
    MarkerMouseHandler &getMarkerMouseHandler();

    const WaveformMouseHandler &getWaveformMouseHandler() const;
    WaveformMouseHandler &getWaveformMouseHandler();

    TransportStrip *getTransportStrip() {
        return topBarView != nullptr ? topBarView->transportStrip.get() : nullptr;
    }
    TopBarView* getTopBarView() { return topBarView.get(); }
    MarkerStrip *getInStrip() { return inStrip.get(); }
    MarkerStrip *getOutStrip() { return outStrip.get(); }
    CutLengthStrip* getCutLengthStrip() { return cutLengthStrip.get(); }
    PlaybackTimeView* getPlaybackTimeView() { return playbackTimeView.get(); }
    SilenceDetectionPresenter *getSilenceDetectionPresenter() {
        return presenterCore != nullptr ? &presenterCore->getSilenceDetectionPresenter() : nullptr;
    }



    juce::TextEditor &getStatsDisplay();

    const juce::LookAndFeel &getLookAndFeel() const;
    FocusManager &getFocusManager() const { return *focusManager; }

    PlaybackTimerManager &getPlaybackTimerManager() { return *playbackTimerManager; }
    PresenterCore &getPresenterCore() { return *presenterCore; }
    const PresenterCore &getPresenterCore() const { return *presenterCore; }
    BoundaryLogicPresenter &getBoundaryLogicPresenter() { return presenterCore->getBoundaryLogicPresenter(); }
    RepeatButtonPresenter &getRepeatButtonPresenter() { return presenterCore->getRepeatButtonPresenter(); }
    MatrixView& getMatrixView() { return topBarView->getMatrixView(); }
    HintView& getHintView() { return topBarView->getHintView(); }
    PlaybackTextPresenter &getPlaybackTextPresenter() { return presenterCore->getPlaybackTextPresenter(); }
    WaveformCanvasView* getWaveformCanvasView() { return waveformCanvasView.get(); }
    FpsView& getFpsView() { return *fpsView; }

    void invokeOwnerOpenDialog();

  private:
    friend class LayoutManager;
    friend class ControlStatePresenter;
    friend class SilenceDetectionPresenter;
    friend class ControlButtonsPresenter;
    friend class CutButtonPresenter;
    friend class CutResetPresenter;
    friend class PlaybackTextPresenter;
    friend class RepeatButtonPresenter;
    friend class BoundaryLogicPresenter;
    friend class ZoomPresenter;
    friend class PresenterCore;

    MainComponent &owner;
    SessionState &sessionState;
    ModernLookAndFeel modernLF;

    // Injected Logic Managers (Raw pointers)
    InteractionCoordinator* interactionCoordinator{nullptr};
    PlaybackTimerManager* playbackTimerManager{nullptr};
    PresenterCore* presenterCore{nullptr};
    FocusManager* focusManager{nullptr};

    std::unique_ptr<LayoutManager> layoutManager;
    std::unique_ptr<WaveformCanvasView> waveformCanvasView;
    std::unique_ptr<OverlayView> overlayView;
    std::unique_ptr<FpsView> fpsView;

    std::unique_ptr<TopBarView> topBarView;
    std::unique_ptr<PlaybackTimeView> playbackTimeView;
    std::unique_ptr<MarkerStrip> inStrip, outStrip;
    std::unique_ptr<CutLengthStrip> cutLengthStrip;

    LayoutCache layoutCache;

    void initialiseLookAndFeel();
    void setupViews();
    void setupStrips();
    void setupListeners();
    void finaliseSetup();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPanel)
};

#endif
