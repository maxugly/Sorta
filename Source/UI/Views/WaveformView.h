

#ifndef AUDIOFILER_WAVEFORMVIEW_H
#define AUDIOFILER_WAVEFORMVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Core/AppEnums.h"
#include "Utils/Config.h"

class WaveformManager;

class WaveformView : public juce::Component, public juce::ChangeListener {
  public:
    explicit WaveformView(WaveformManager &waveformManager);

    ~WaveformView() override;

    void setChannelMode(AppEnums::ChannelViewMode channelMode);

    void paint(juce::Graphics &g) override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void clearCaches();

  private:
    void drawWaveform(juce::Graphics &g);

    WaveformManager &waveformManager;
    AppEnums::ChannelViewMode currentChannelMode = AppEnums::ChannelViewMode::Mono;

    juce::Image cachedWaveform;
    bool isCacheDirty{true};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView)
};

#endif
