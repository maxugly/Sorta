

#ifndef AUDIOFILER_ZOOMVIEW_H
#define AUDIOFILER_ZOOMVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"

#include "Presenters/PlaybackTimerManager.h"

class ControlPanel;

struct ZoomViewState {
    int mouseX{-1};
    int mouseY{-1};
    juce::String mouseTimeText;
    float amplitude{0.0f};
    bool isZooming{false};
    juce::Rectangle<int> popupBounds;
    double startTime{0.0};
    double endTime{0.0};
    float eyeCandyPulse{0.0f};

    double audioLength{0.0};
    AppEnums::PlacementMode placementMode{AppEnums::PlacementMode::None};
    bool isZKeyDown{false};
    AppEnums::ChannelViewMode channelMode{AppEnums::ChannelViewMode::Mono};
    int numChannels{0};
    
    float cutInPixelX{0.0f};
    float cutOutPixelX{0.0f};
    float currentPositionPixelX{0.0f};
    
    bool isDraggingCutIn{false};
    bool isDraggingCutOut{false};
    bool shouldShowEyeCandy{false};
    double mouseTime{0.0};
    juce::AudioThumbnail* thumbnail{nullptr};
};

class ZoomView : public juce::Component {
  public:
    explicit ZoomView(ControlPanel &owner);
    ~ZoomView() override;

    void paint(juce::Graphics &g) override;

    void updateState(const ZoomViewState& newState) {
        state = newState;
        repaint();
    }

  private:
    ControlPanel &owner;
    ZoomViewState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomView)
};

#endif
