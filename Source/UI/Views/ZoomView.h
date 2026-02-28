#ifndef AUDIOFILER_ZOOMVIEW_H
#define AUDIOFILER_ZOOMVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Presenters/PlaybackTimerManager.h"

/**
 * @file ZoomView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief View component responsible for rendering the high-detail zoom popup and HUD.
 */

class ControlPanel;

/**
 * @struct ZoomHudLine
 * @brief Represents a single line of text in the Zoom HUD overlay.
 */
struct ZoomHudLine {
    /** @brief The text content to display in the HUD. */
    juce::String text;
    /** @brief True if this line should be rendered with an 'active' or 'highlighted' style. */
    bool isActive{false};
};

/**
 * @struct ZoomViewState
 * @brief Encapsulates the complete visual state required to render the zoom popup and HUD.
 * 
 * @details This struct acts as a "Passive Data Model" for the ZoomView. It 
 *          contains all calculated coordinates, time strings, and flags 
 *          necessary for the view to render the zoom preview and cursor 
 *          crosshairs without performing any business logic.
 */
struct ZoomViewState {
    /** @brief The horizontal pixel X coordinate of the mouse. */
    int mouseX{-1};
    /** @brief The vertical pixel Y coordinate of the mouse. */
    int mouseY{-1};
    /** @brief The formatted time string corresponding to the mouse position. */
    juce::String mouseTimeText;
    /** @brief The vertical pixel Y coordinate for the positive amplitude crosshair. */
    float amplitudeY{0.0f};
    /** @brief The vertical pixel Y coordinate for the negative amplitude crosshair. */
    float bottomAmplitudeY{0.0f};
    /** @brief The formatted positive amplitude string (e.g., in dB or %). */
    juce::String amplitudeText;
    /** @brief The formatted negative amplitude string. */
    juce::String negAmplitudeText;
    /** @brief Flag indicating if the zoom popup should currently be visible. */
    bool isZooming{false};
    /** @brief The screen-space bounds where the zoom popup should be rendered. */
    juce::Rectangle<int> popupBounds;
    /** @brief The start time of the zoomed segment in seconds. */
    double startTime{0.0};
    /** @brief The end time of the zoomed segment in seconds. */
    double endTime{0.0};

    /** @brief The total duration of the audio in seconds. */
    double audioLength{0.0};
    /** @brief The current placement mode (In/Out/None). */
    AppEnums::PlacementMode placementMode{AppEnums::PlacementMode::None};
    /** @brief Flag indicating if the 'Z' key is currently held. */
    bool isZKeyDown{false};
    /** @brief The current channel view mode (Mono/Stereo). */
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};
    /** @brief The number of audio channels being displayed. */
    int numChannels{0};
    
    /** @brief The pixel X coordinate of the 'In' point within the zoom window. */
    float cutInPixelX{0.0f};
    /** @brief The pixel X coordinate of the 'Out' point within the zoom window. */
    float cutOutPixelX{0.0f};
    /** @brief The pixel X coordinate of the playhead within the zoom window. */
    float currentPositionPixelX{0.0f};
    
    /** @brief True if the 'In' marker is currently being dragged within the zoom view. */
    bool isDraggingCutIn{false};
    /** @brief True if the 'Out' marker is currently being dragged within the zoom view. */
    bool isDraggingCutOut{false};
    /** @brief The precise time in seconds under the mouse cursor. */
    double mouseTime{0.0};
    /** @brief Pointer to the audio thumbnail used for high-detail rendering. */
    juce::AudioThumbnail* thumbnail{nullptr};
    /** @brief The collection of lines to render in the status HUD. */
    std::vector<ZoomHudLine> hudLines;

    /** @brief The color of the mouse cursor crosshairs. */
    juce::Colour cursorLineColor;
    /** @brief The highlight color for the cursor focal point. */
    juce::Colour cursorHighlightColor;
    /** @brief The color of the outer cursor glow. */
    juce::Colour cursorGlowColor;
    /** @brief The thickness of the cursor glow in pixels. */
    float cursorGlowThickness{0.0f};
};

/**
 * @class ZoomView
 * @brief A passive UI component that renders high-detail zoom popups and real-time cursor crosshairs.
 * 
 * @details Architecturally, the ZoomView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the high-detail waveform segment 
 *          and cursor HUD. It utilizes an intelligent "Dirty Rectangle" 
 *          repainting strategy to ensure high-performance updates for the 
 *          cursor and HUD without redrawing the entire canvas. It relies 
 *          entirely on the ZoomPresenter to calculate and push its visual 
 *          state (via ZoomViewState).
 * 
 * @see ZoomPresenter, WaveformCanvasView, ZoomViewState
 */
class ZoomView : public juce::Component {
  public:
    /**
     * @brief Constructs a new ZoomView.
     * @param owner Reference to the parent ControlPanel.
     */
    explicit ZoomView(ControlPanel &owner);

    /** @brief Destructor. */
    ~ZoomView() override;

    /** @brief Standard JUCE paint callback, rendering the crosshairs, HUD, and popup. */
    void paint(juce::Graphics &g) override;

    /** 
     * @brief Updates the view's internal state and performs optimized repaints. 
     * @details This method implements a "Dirty Rectangle" optimization to minimize 
     *          GPU/CPU load during high-frequency updates like mouse movement.
     * @param newState The new visual state to apply.
     */
    void updateState(const ZoomViewState& newState) {
        // 1. Only do a full repaint for major mode changes
        if (state.isZooming != newState.isZooming ||
            state.isZKeyDown != newState.isZKeyDown ||
            state.placementMode != newState.placementMode ||
            state.audioLength != newState.audioLength ||
            state.channelMode != newState.channelMode ||
            state.startTime != newState.startTime ||
            state.endTime != newState.endTime ||
            state.popupBounds != newState.popupBounds) {
            
            if (state.startTime != newState.startTime || 
                state.endTime != newState.endTime || 
                state.popupBounds != newState.popupBounds ||
                state.channelMode != newState.channelMode) 
            {
                isCacheDirty = true;
            }

            state = newState;
            repaint(); 
            return;
        }

        // 2. Dirty Rectangles: Only repaint the popup box if inner lines move
        if (state.currentPositionPixelX != newState.currentPositionPixelX ||
            state.cutInPixelX != newState.cutInPixelX ||
            state.cutOutPixelX != newState.cutOutPixelX) {
            if (state.isZooming) repaint(state.popupBounds);
            if (newState.isZooming) repaint(newState.popupBounds);
        }

        // 3. Dirty Rectangles: Only erase and redraw the crosshair regions
        if (state.mouseX != newState.mouseX || state.mouseY != newState.mouseY) {
            auto invalidateCrosshair = [this](const ZoomViewState& s) {
                if (s.mouseX == -1) return;
                repaint(s.mouseX - 20, 0, 150, getHeight()); // Vertical crosshair + text
                repaint(0, s.mouseY - 10, getWidth(), 20);   // Horizontal crosshair
                if (s.amplitudeY > 0) {
                    repaint(0, (int)s.amplitudeY - 20, getWidth(), 40);
                    repaint(0, (int)s.bottomAmplitudeY - 20, getWidth(), 40);
                }
            };

            invalidateCrosshair(state);    // Erase old position
            invalidateCrosshair(newState); // Redraw new position
        }
        
        state = newState;
    }

  private:
    /** @brief Renders the real-time cursor crosshairs and amplitude markers. */
    void drawMouseCursor(juce::Graphics& g);
    /** @brief Renders the high-detail zoom preview window. */
    void drawZoomPopup(juce::Graphics& g);
    /** @brief Renders the status and metadata HUD overlay. */
    void drawHud(juce::Graphics& g);

    ControlPanel &owner;
    ZoomViewState state;
    juce::Image waveformCache;
    bool isCacheDirty{true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomView)
};

#endif
