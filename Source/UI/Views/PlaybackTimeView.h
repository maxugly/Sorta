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

    juce::TextEditor& getElapsedEditor() { return elapsedTimeEditor; }
    juce::TextEditor& getRemainingEditor() { return remainingTimeEditor; }
    juce::TextEditor& getCutLengthEditor() { return cutLengthEditor; }

  private:
    juce::TextEditor elapsedTimeEditor;
    juce::TextEditor remainingTimeEditor;
    juce::TextEditor cutLengthEditor;
    juce::String totalTimeStaticStr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackTimeView)
};

#endif
