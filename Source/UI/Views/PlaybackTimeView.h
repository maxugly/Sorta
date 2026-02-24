/**
 * @file PlaybackTimeView.h
 * @Source/repo_files_list.txt
 * @ingroup UI
 * @brief Passive view for playback time and cut length displays.
 */

#ifndef AUDIOFILER_PLAYBACKTIMEVIEW_H
#define AUDIOFILER_PLAYBACKTIMEVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

class PlaybackTimeView final : public juce::Component {
  public:
    PlaybackTimeView();
    ~PlaybackTimeView() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void setTotalTimeStaticString(const juce::String& text) {
        totalTimeStaticStr = text;
        repaint();
    }

    void updateTimes(const juce::String& current, const juce::String& total) {
        if (currentTimeString == current && totalTimeString == total) return;
        currentTimeString = current;
        totalTimeString = total;
        repaint();
    }

    juce::TextEditor& getElapsedEditor() { return elapsedTimeEditor; }
    juce::TextEditor& getRemainingEditor() { return remainingTimeEditor; }

  private:
    juce::TextEditor elapsedTimeEditor;
    juce::TextEditor remainingTimeEditor;
    juce::String totalTimeStaticStr;

    juce::String currentTimeString;
    juce::String totalTimeString;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackTimeView)
};

#endif
