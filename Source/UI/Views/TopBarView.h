/**
 * @file TopBarView.h
 * @Source/repo_files_list.txt
 * @ingroup UI
 * @brief Container for the top row of control buttons.
 */

#ifndef AUDIOFILER_TOPBARVIEW_H
#define AUDIOFILER_TOPBARVIEW_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include <memory>
#include "UI/Components/TransportButton.h"
#include "UI/Views/MatrixView.h"
#include "UI/Views/HintView.h"

class ControlPanel;
class TransportStrip;
class VolumeView;

class TopBarView final : public juce::Component {
  public:
    explicit TopBarView(ControlPanel &cp);
    ~TopBarView() override;

    void resized() override;

    VolumeView& getVolumeView() { return *volumeView; }
    MatrixView& getMatrixView() { return *matrixView; }
    HintView& getHintView() { return *hintView; }

    TransportButton openButton;
    TransportButton modeButton;
    TransportButton exitButton;
    TransportButton statsButton;
    TransportButton channelViewButton;

    // We keep transportStrip here to manage the top row layout entirely
    std::unique_ptr<TransportStrip> transportStrip;
    std::unique_ptr<VolumeView> volumeView;
    std::unique_ptr<MatrixView> matrixView;
    std::unique_ptr<HintView> hintView;

  private:
    ControlPanel &controlPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBarView)
};

#endif
