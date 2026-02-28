#ifndef AUDIOFILER_STATSPRESENTER_H
#define AUDIOFILER_STATSPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Utils/Config.h"
#include "Core/SessionState.h"

class ControlPanel;

/**
 * @file StatsPresenter.h
 * @ingroup Logic
 * @brief Presenter and View for displaying technical audio file metadata.
 */

/**
 * @class StatsOverlay
 * @ingroup UI
 * @brief A passive, resizable view for displaying file statistics.
 * 
 * @details Architecturally, StatsOverlay is a "Passive View" that serves as 
 *          a transparent container for a read-only `juce::TextEditor`. It 
 *          implements its own resizing logic via `juce::ResizableEdgeComponent`, 
 *          allowing users to pull down the metadata tray to reveal more info.
 * 
 *          This component remains "dumb"—it has no knowledge of where the 
 *          text comes from or what it represents. It simply renders the 
 *          string provided by the `StatsPresenter`.
 * 
 * @see StatsPresenter
 */
class StatsOverlay : public juce::Component {
  public:
    /**
     * @brief Constructs the overlay and initializes its sub-components.
     */
    StatsOverlay() : resizer(this, &constrainer, juce::ResizableEdgeComponent::bottomEdge) {
        setInterceptsMouseClicks(false, true);

        addAndMakeVisible(statsDisplay);
        addAndMakeVisible(resizer);

        statsDisplay.setInterceptsMouseClicks(false, false);
        statsDisplay.setReadOnly(true);
        statsDisplay.setMultiLine(true);

        constrainer.setMinimumHeight(Config::Layout::Stats::minHeight);
        constrainer.setMaximumHeight(Config::Layout::Stats::maxHeight);
    }

    /**
     * @brief Renders the background tray and the "grab handle" for resizing.
     */
    void paint(juce::Graphics &g) override {
        g.setColour(Config::Colors::statsBackground);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), Config::Layout::Stats::cornerRadius);

        auto handleArea =
            getLocalBounds().removeFromBottom(Config::Layout::Stats::handleAreaHeight);
        g.setColour(Config::Colors::Button::text);

        // Render the decorative "grab lines"
        g.fillRect(handleArea
                       .withSizeKeepingCentre(Config::Layout::Stats::handleWidth,
                                              Config::Layout::Stats::handleLineHeight)
                       .translated(0, -1));
        g.fillRect(handleArea
                       .withSizeKeepingCentre(Config::Layout::Stats::handleWidth,
                                              Config::Layout::Stats::handleLineHeight)
                       .translated(0, 2));
    }

    /**
     * @brief Updates the bounds of the text editor and resizer handle.
     */
    void resized() override {
        auto b = getLocalBounds();
        auto handleArea = b.removeFromBottom(Config::Layout::Stats::handleAreaHeight);
        statsDisplay.setBounds(b.reduced(Config::Layout::Stats::internalPadding));
        resizer.setBounds(handleArea);
        if (onHeightChanged)
            onHeightChanged(getHeight());
    }

    std::function<void(int)> onHeightChanged; /**< Callback for persistent height tracking. */
    juce::TextEditor statsDisplay;           /**< The actual read-only text container. */
    juce::ResizableEdgeComponent resizer;     /**< Interactive handle for height adjustment. */

  private:
    juce::ComponentBoundsConstrainer constrainer; /**< Enforces min/max height limits. */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatsOverlay)
};

/**
 * @class StatsPresenter
 * @ingroup Logic
 * @brief Presenter responsible for technical metadata extraction and display.
 * 
 * @details Architecturally, StatsPresenter acts as the "Presenter Glue" that 
 *          interrogates the `AudioPlayer` and `SessionState` to build a 
 *          technical summary of the currently loaded asset. 
 * 
 *          It handles:
 *          - **Metadata Extraction**: Gathers sample rate, bit depth, channel 
 *            count, and peak amplitude data.
 *          - **Lifecycle Monitoring**: Observes `SessionState::fileChanged` to 
 *            automatically refresh the display when a new asset is loaded.
 *          - **Visibility Management**: Toggles the metadata tray in response 
 *            to user keybinds or menu actions.
 * 
 * @see StatsOverlay
 * @see AudioPlayer
 * @see SessionState
 */
class StatsPresenter final : public SessionState::Listener {
  public:
    /**
     * @brief Constructs the presenter and wires it to the parent view.
     * @param owner Reference to the main ControlPanel.
     */
    explicit StatsPresenter(ControlPanel &owner);

    /**
     * @brief Ensures clean listener removal.
     */
    ~StatsPresenter() override;

    /**
     * @brief Forces a refresh of the technical statistics string.
     */
    void updateStats();

    /**
     * @brief Toggles the visibility of the StatsOverlay.
     */
    void toggleVisibility();

    /**
     * @brief Explicitly sets the visibility state of the overlay.
     * @param shouldShowStats True to animate the tray into view.
     */
    void setShouldShowStats(bool shouldShowStats);

    /**
     * @brief Queries the current visibility state.
     * @return True if the overlay is active.
     */
    bool isShowingStats() const noexcept {
        return showStats;
    }

    /**
     * @brief Retrieves the raw formatted statistics string.
     * @return Multi-line metadata string.
     */
    juce::String getStatsText() const;

    /**
     * @brief Positions the overlay within the provided parent bounds.
     * @param contentAreaBounds The rectangle representing the available UI space.
     */
    void layoutWithin(const juce::Rectangle<int> &contentAreaBounds);

    /**
     * @brief Manually updates the displayed text (e.g., for error reporting).
     * @param text The message to show.
     * @param color The text color (defaults to theme primary).
     */
    void setDisplayText(const juce::String &text, juce::Colour color = Config::Colors::statsText);

    /**
     * @brief Provides direct access to the internal TextEditor.
     * @return Reference to the overlay's display component.
     */
    juce::TextEditor &getDisplay();

    /**
     * @brief Toggles whether the display component can receive focus.
     */
    void setDisplayEnabled(bool shouldEnable);

    /**
     * @brief Triggers a stats refresh when a new file is loaded.
     * @param filePath Path to the new asset.
     */
    void fileChanged(const juce::String &filePath) override;

  private:
    /**
     * @brief Mathematical internal helper to construct the technical summary.
     * @return Formatted multi-line string.
     */
    juce::String buildStatsString() const;

    /**
     * @brief Synchronizes the StatsOverlay's visibility with the internal state.
     */
    void updateVisibility();

    ControlPanel &owner;        /**< Reference to the parent view shell. */
    StatsOverlay statsOverlay;  /**< The passive view managed by this presenter. */
    bool showStats{false};      /**< Current visibility flag. */
    int currentHeight{Config::Layout::Stats::initialHeight}; /**< User-defined height for the tray. */
};

#endif
