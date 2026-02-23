
#ifndef AUDIOFILER_MATRIXVIEW_H
#define AUDIOFILER_MATRIXVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <vector>

struct MatrixViewState {
    std::vector<juce::Colour> ledColors;
};

class MatrixView : public juce::Component {
  public:
    MatrixView();
    ~MatrixView() override;

    void updateState(const MatrixViewState& newState);
    void paint(juce::Graphics& g) override;

  private:
    MatrixViewState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixView)
};

#endif
