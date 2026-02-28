#ifndef AUDIOFILER_MATRIXVIEW_H
#define AUDIOFILER_MATRIXVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <vector>

/**
 * @file MatrixView.h
 * @ingroup UI
 * @brief View responsible for rendering the auxiliary LED-style status matrix.
 */

/**
 * @struct MatrixViewState
 * @brief Simple data container representing the visual state of the LED matrix.
 */
struct MatrixViewState {
    /** @brief A list of colors for each individual "pixel" in the matrix. */
    std::vector<juce::Colour> ledColors;
};

/**
 * @class MatrixView
 * @ingroup UI
 * @brief A passive, "dumb" component for rendering the LED status grid.
 * 
 * @details Architecturally, MatrixView serves as a "Passive View" that 
 *          implements the display layer of the LED matrix feature. It 
 *          follows the "Passive View" mandate by containing zero business 
 *          logic—it does not know what the LEDs represent (e.g., volume, 
 *          playback state, or errors). It merely iterates through its 
 *          internal `MatrixViewState` and paints the corresponding colors.
 * 
 *          Updates are pushed to the view via the `updateState()` method, 
 *          typically by the `MatrixPresenter`.
 * 
 * @see MatrixPresenter
 * @see MatrixViewState
 * @see ModernLookAndFeel
 */
class MatrixView : public juce::Component {
  public:
    /**
     * @brief Constructs the view and initializes the internal state grid.
     */
    MatrixView();

    /**
     * @brief Cleans up resources.
     */
    ~MatrixView() override;

    /**
     * @brief Replaces the current visual state and triggers a repaint.
     * @param newState The new color grid to render.
     */
    void updateState(const MatrixViewState& newState);

    /**
     * @brief Renders the LED grid based on the current state.
     * @details Mathematical approach: 
     *          The matrix is rendered as a series of circles/squares 
     *          arranged in a grid. Each LED is centered within its 
     *          calculated cell using a fixed `squareSize` from `Config::Layout::Matrix`.
     * @param g The graphics context to paint into.
     */
    void paint(juce::Graphics& g) override;

  private:
    MatrixViewState state;      /**< The current data-driven state being rendered. */
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixView)
};

#endif
