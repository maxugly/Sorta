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

/**
 * @file ControlPanel.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief The visual container for UI controls and waveform visualization.
 */

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
 * @class ControlPanel
 * @brief The visual container for all UI controls and waveform visualization.
 * 
 * @details Architecturally, the ControlPanel serves as a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business logic 
 *          and is responsible only for managing the lifecycle, layout, and rendering 
 *          of its child components. It relies entirely on injected Presenters (via 
 *          PresenterCore) and logic managers (InteractionCoordinator, FocusManager) 
 *          to handle state updates and user interactions. This decoupling ensures that 
 *          the UI remains focused purely on paint() and resized() operations.
 * 
 * @see PresenterCore, InteractionCoordinator, LayoutManager, WaveformCanvasView
 */
class ControlPanel final : public juce::Component {
  public:
    /** 
     * @struct LayoutCache
     * @brief Stores pre-calculated bounds for performance-critical UI segments.
     */
    struct LayoutCache {
        /** @brief The calculated bounds for the main waveform rendering area. */
        juce::Rectangle<int> waveformBounds;
        /** @brief The calculated bounds for the overall content area. */
        juce::Rectangle<int> contentAreaBounds;
    };

    /**
     * @brief Constructs a new ControlPanel.
     * @param owner The parent MainComponent shell.
     * @param sessionStateIn The application session state.
     */
    explicit ControlPanel(MainComponent &owner, SessionState &sessionStateIn);

    /** @brief Destructor. */
    ~ControlPanel() override;

    /** 
     * @brief Injects the required logic managers into the panel. 
     * @param ic The interaction coordinator.
     * @param ptm The playback timer manager.
     * @param pc The core presenter suite.
     * @param fm The focus manager.
     */
    void injectLogic(InteractionCoordinator& ic, PlaybackTimerManager& ptm, PresenterCore& pc, FocusManager& fm);

    /** @brief Sets the interaction coordinator manually. */
    void setInteractionCoordinator(InteractionCoordinator& ic) { interactionCoordinator = &ic; }

    /** @brief Sets the playback timer manager manually. */
    void setPlaybackTimerManager(PlaybackTimerManager& ptm) { playbackTimerManager = &ptm; }

    /** @brief Sets the core presenter suite manually. */
    void setPresenterCore(PresenterCore& pc) { presenterCore = &pc; }

    /** @brief Sets the focus manager manually. */
    void setFocusManager(FocusManager& fm) { focusManager = &fm; }

    /** @brief Standard JUCE paint callback. */
    void paint(juce::Graphics &g) override;

    /** @brief Standard JUCE resized callback, delegating layout to LayoutManager. */
    void resized() override;

    /** @brief Determines the appropriate mouse cursor for the current state. */
    juce::MouseCursor getMouseCursor() override;

    /** @return Reference to the 'In' point auto-cut button. */
    juce::TextButton &getAutoCutInButton() { return inStrip->getAutoCutButton(); }

    /** @return Reference to the 'Out' point auto-cut button. */
    juce::TextButton &getAutoCutOutButton() { return outStrip->getAutoCutButton(); }

    /** @return The pre-calculated waveform bounds from the layout cache. */
    juce::Rectangle<int> getWaveformBounds() const { return layoutCache.waveformBounds; }

    /** @return Reference to the active audio player engine. */
    AudioPlayer &getAudioPlayer();
    /** @return Const reference to the active audio player engine. */
    AudioPlayer &getAudioPlayer() const;

    /** @return Reference to the current session state model. */
    SessionState &getSessionState() { return sessionState; }
    /** @return Const reference to the current session state model. */
    const SessionState &getSessionState() const { return sessionState; }

    /** @return Reference to the interaction coordinator. */
    InteractionCoordinator &getInteractionCoordinator() { return *interactionCoordinator; }

    /** @return The current channel view mode (Mono/Stereo). */
    AppEnums::ChannelViewMode getChannelViewMode() const { return sessionState.getChannelViewMode(); }

    /** @return Reference to the marker mouse handler. */
    const MarkerMouseHandler &getMarkerMouseHandler() const;
    /** @return Mutable reference to the marker mouse handler. */
    MarkerMouseHandler &getMarkerMouseHandler();

    /** @return Reference to the waveform mouse handler. */
    const WaveformMouseHandler &getWaveformMouseHandler() const;
    /** @return Mutable reference to the waveform mouse handler. */
    WaveformMouseHandler &getWaveformMouseHandler();

    /** @return Pointer to the transport strip component. */
    TransportStrip *getTransportStrip() {
        return topBarView != nullptr ? topBarView->transportStrip.get() : nullptr;
    }

    /** @return Pointer to the top bar view. */
    TopBarView* getTopBarView() { return topBarView.get(); }

    /** @return Pointer to the 'In' marker strip. */
    MarkerStrip *getInStrip() { return inStrip.get(); }

    /** @return Pointer to the 'Out' marker strip. */
    MarkerStrip *getOutStrip() { return outStrip.get(); }

    /** @return Pointer to the cut length display strip. */
    CutLengthStrip* getCutLengthStrip() { return cutLengthStrip.get(); }

    /** @return Pointer to the playback time display view. */
    PlaybackTimeView* getPlaybackTimeView() { return playbackTimeView.get(); }

    /** @return Pointer to the silence detection presenter. */
    SilenceDetectionPresenter *getSilenceDetectionPresenter() {
        return presenterCore != nullptr ? &presenterCore->getSilenceDetectionPresenter() : nullptr;
    }

    /** @return Reference to the statistics text editor. */
    juce::TextEditor &getStatsDisplay();

    /** @return Const reference to the panel's look and feel. */
    const juce::LookAndFeel &getLookAndFeel() const;

    /** @return Reference to the focus manager. */
    FocusManager &getFocusManager() const { return *focusManager; }

    /** @return Reference to the playback timer manager. */
    PlaybackTimerManager &getPlaybackTimerManager() { return *playbackTimerManager; }

    /** @return Reference to the core presenter suite. */
    PresenterCore &getPresenterCore() { return *presenterCore; }
    /** @return Const reference to the core presenter suite. */
    const PresenterCore &getPresenterCore() const { return *presenterCore; }

    /** @return Reference to the boundary logic presenter. */
    BoundaryLogicPresenter &getBoundaryLogicPresenter() { return presenterCore->getBoundaryLogicPresenter(); }

    /** @return Reference to the repeat button presenter. */
    RepeatButtonPresenter &getRepeatButtonPresenter() { return presenterCore->getRepeatButtonPresenter(); }

    /** @return Reference to the matrix view. */
    MatrixView& getMatrixView() { return topBarView->getMatrixView(); }

    /** @return Reference to the hint view. */
    HintView& getHintView() { return topBarView->getHintView(); }

    /** @return Reference to the playback text presenter. */
    PlaybackTextPresenter &getPlaybackTextPresenter() { return presenterCore->getPlaybackTextPresenter(); }

    /** @return Pointer to the waveform canvas view. */
    WaveformCanvasView* getWaveformCanvasView() { return waveformCanvasView.get(); }

    /** @return Reference to the FPS display view. */
    FpsView& getFpsView() { return *fpsView; }

    /** @brief Triggers the owner's file open dialog. */
    void invokeOwnerOpenDialog();

    /** @brief Initialises the custom modern look and feel. */
    void initialiseLookAndFeel();

    /** @brief Refreshes the theme colors in real-time. */
    void refreshThemeLive();

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

    /** @brief Sets up and child views. */
    void setupViews();
    /** @brief Sets up the marker and transport strips. */
    void setupStrips();
    /** @brief Sets up component listeners. */
    void setupListeners();
    /** @brief Finalises the UI assembly. */
    void finaliseSetup();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPanel)
};

#endif
