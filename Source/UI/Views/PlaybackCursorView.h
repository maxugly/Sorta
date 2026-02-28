#ifndef AUDIOFILER_PLAYBACKCURSORVIEW_H
#define AUDIOFILER_PLAYBACKCURSORVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file PlaybackCursorView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief View responsible for rendering the animated playback playhead.
 */

class ControlPanel;

/**
 * @struct PlaybackCursorViewState
 * @brief Encapsulates the visual state required to render the playback playhead.
 */
struct PlaybackCursorViewState {
    /** @brief The horizontal pixel X coordinate of the playhead. */
    float playheadX{0.0f};
    /** @brief True if the playhead should be visible. */
    bool isVisible{false};
    /** @brief The color used for the playhead marker caps. */
    juce::Colour headColor;
    /** @brief The color used for the playhead center line. */
    juce::Colour centerLineColor;
};

/**
 * @class PlaybackCursorView
 * @brief A passive UI component that renders the animated playback playhead.
 * 
 * @details Architecturally, PlaybackCursorView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the playhead at the position 
 *          provided by its presenter. It relies entirely on the CutPresenter 
 *          to calculate its state and push updates via updateState().
 * 
 * @see CutPresenter, WaveformCanvasView, PlaybackCursorViewState
 */
class PlaybackCursorView : public juce::Component {
  public:
    /**
     * @brief Constructs a new PlaybackCursorView.
     * @param owner Reference to the parent ControlPanel.
     */
    explicit PlaybackCursorView(ControlPanel &owner);

    /** @brief Destructor. */
    ~PlaybackCursorView() override;

    /** @brief Standard JUCE paint callback. */
    void paint(juce::Graphics &g) override;

    /** 
     * @brief Updates the view's internal state and triggers a repaint. 
     * @param newState The new visual state to apply.
     */
    void updateState(const PlaybackCursorViewState& newState);

  private:
    ControlPanel &owner;
    PlaybackCursorViewState state;
    int lastCursorX = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackCursorView)
};

#endif
