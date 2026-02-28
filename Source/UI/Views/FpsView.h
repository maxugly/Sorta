#ifndef AUDIOFILER_FPSVIEW_H
#define AUDIOFILER_FPSVIEW_H

#if !defined(JUCE_HEADLESS)
#include <JuceHeader.h>
#endif

/**
 * @file FpsView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Passive view for displaying frame-rate performance statistics.
 */

/**
 * @class FpsView
 * @brief A passive UI component for displaying current FPS and frame times.
 * 
 * @details Architecturally, FpsView is a "Passive View" in the Model-View-Presenter (MVP) 
 *          hierarchy. It merely displays the performance data calculated 
 *          and pushed by the FpsPresenter. It contains no business logic 
 *          regarding performance metrics.
 * 
 * @see FpsPresenter
 */
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
