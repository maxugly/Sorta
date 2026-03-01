#ifndef AUDIOFILER_PLAYBACKTIMEVIEW_H
#define AUDIOFILER_PLAYBACKTIMEVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "UI/Components/StyledTextEditor.h"

/**
 * @file PlaybackTimeView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Passive view for playback time and cut length displays.
 */

/**
 * @class PlaybackTimeView
 * @brief A passive UI component that displays the current playback time, remaining time, and total duration.
 * 
 * @details Architecturally, the PlaybackTimeView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to render the formatted time strings provided 
 *          by its associated presenter. It uses StyledTextEditor components 
 *          for its display areas to maintain consistent application styling. 
 *          It relies entirely on the PlaybackTextPresenter to calculate 
 *          and push time updates.
 * 
 * @see PlaybackTextPresenter, ControlPanel, StyledTextEditor
 */
class PlaybackTimeView final : public juce::Component {
  public:
    /** @brief Constructs a new PlaybackTimeView and initialises its editors. */
    PlaybackTimeView();

    /** @brief Destructor. */
    ~PlaybackTimeView() override;

    /** @brief Standard JUCE paint callback. */
    void paint(juce::Graphics &g) override;

    /** @brief Standard JUCE resized callback, organizing the time display editors. */
    void resized() override;

    /** 
     * @brief Toggles the layout mode between centered and split (corners).
     * @param shouldSplit True to snap editors to corners, false to center.
     */
    void setSplitLayout(bool shouldSplit);

    /** 
     * @brief Sets the static total time string. 
     * @param text The formatted total time string.
     */
    void setTotalTimeStaticString(const juce::String& text) {
        totalTimeStaticStr = text;
        repaint();
    }

    /** 
     * @brief Updates the current and total time display strings. 
     * @param current The formatted current playback time string.
     * @param total The formatted total duration string.
     */
    void updateTimes(const juce::String& current, const juce::String& total) {
        if (currentTimeString == current && totalTimeString == total) return;
        currentTimeString = current;
        totalTimeString = total;
        repaint();
    }

    /** @return Reference to the elapsed time text editor. */
    StyledTextEditor& getElapsedEditor() { return elapsedTimeEditor; }

    /** @return Reference to the remaining time text editor. */
    StyledTextEditor& getRemainingEditor() { return remainingTimeEditor; }

    /** @return Reference to the total duration text editor. */
    StyledTextEditor& getTotalTimeEditor() { return totalTimeEditor; }

  private:
    StyledTextEditor elapsedTimeEditor;
    StyledTextEditor remainingTimeEditor;
    StyledTextEditor totalTimeEditor;
    juce::String totalTimeStaticStr;

    juce::String currentTimeString;
    juce::String totalTimeString;

    bool isSplit{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackTimeView)
};

#endif
