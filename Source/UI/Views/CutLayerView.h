

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
    float inThresholdYTop{0.0f};
    float inThresholdYBottom{0.0f};
    float outThresholdYTop{0.0f};
    float outThresholdYBottom{0.0f};
    float fadeWidthPixels{0.0f};
    float audioLength{0.0};
    float glowAlpha{0.0f};
    bool showEyeCandy{false};
    bool isAutoIn{false};
    bool isAutoOut{false};
    bool markersVisible{false};
    MarkerMouseHandler::CutMarkerHandle draggedHandle{MarkerMouseHandler::CutMarkerHandle::None};
    MarkerMouseHandler::CutMarkerHandle hoveredHandle{MarkerMouseHandler::CutMarkerHandle::None};
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};
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
