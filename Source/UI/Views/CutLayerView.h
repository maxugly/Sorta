

#ifndef AUDIOFILER_CUTLAYERVIEW_H
#define AUDIOFILER_CUTLAYERVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Utils/Config.h"
#include "UI/Handlers/MarkerMouseHandler.h"

class ControlPanel;

struct CutLayerState {
    float inPixelX{0.0f};
    float outPixelX{0.0f};
    float actualInX{0.0f};
    float actualOutX{0.0f};
    float inThresholdYTop{0.0f};
    float inThresholdYBottom{0.0f};
    float outThresholdYTop{0.0f};
    float outThresholdYBottom{0.0f};
    float fadeWidthPixels{0.0f};
    float audioLength{0.0};
    float glowAlpha{0.0f};
    bool showEyeCandy{false};
    bool markersVisible{false};
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};

    juce::Colour inMarkerColor;
    float inMarkerThickness{0.0f};
    bool inMarkerShouldPulse{false};

    juce::Colour outMarkerColor;
    float outMarkerThickness{0.0f};
    bool outMarkerShouldPulse{false};

    juce::Colour regionOutlineColor;
    float regionOutlineThickness{0.0f};
    bool regionShouldPulse{false};
};

class CutLayerView : public juce::Component {
  public:
    explicit CutLayerView(ControlPanel &owner);

    ~CutLayerView() override;

    void updateState(const CutLayerState& newState);

    ControlPanel& getOwner() { return owner; }

    void paint(juce::Graphics &g) override;

  private:
    ControlPanel &owner;
    CutLayerState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutLayerView)
};

#endif
