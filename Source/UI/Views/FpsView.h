#ifndef AUDIOFILER_FPSVIEW_H
#define AUDIOFILER_FPSVIEW_H

#if !defined(JUCE_HEADLESS)
#include <JuceHeader.h>
#endif

class FpsView final : public juce::Component {
public:
    FpsView();
    ~FpsView() override = default;
    void setFps(int fps, double frameTimeMs);
    void paint(juce::Graphics& g) override;
private:
    juce::String currentFpsText{"0 FPS (0.0ms)"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FpsView)
};
#endif
