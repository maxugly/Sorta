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

/**
 * @file TopBarView.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Container view for the top row of controls, navigation, and settings.
 */

class ControlPanel;
class TransportStrip;
class VolumeView;

/**
 * @class TopBarView
 * @brief A passive UI container for the top control bar, housing navigation, settings, and status views.
 * 
 * @details Architecturally, the TopBarView is a "Passive View" or "Dumb Component" 
 *          within the Model-View-Presenter (MVP) law. It contains zero business 
 *          logic and exists purely to organize and render the top row of buttons 
 *          (Open, Settings, Exit), the transport strip, and status views like 
 *          the MatrixView and HintView. It relies entirely on its parent 
 *          ControlPanel and specialized presenters to manage the interaction 
 *          states of its child components. This layout-focused design ensures 
 *          that the top-level navigation remains decoupled from the core 
 *          audio processing logic.
 * 
 * @see ControlPanel, TransportStrip, VolumeView, MatrixView, HintView
 */
class TopBarView final : public juce::Component {
  public:
    /**
     * @brief Constructs a new TopBarView and initialises its child components.
     * @param cp Reference to the parent ControlPanel.
     */
    explicit TopBarView(ControlPanel &cp);

    /** @brief Destructor. */
    ~TopBarView() override;

    /** @brief Standard JUCE resized callback, organizing the top bar layout. */
    void resized() override;

    /** @return Reference to the volume control view. */
    VolumeView& getVolumeView() { return *volumeView; }

    /** @return Reference to the matrix status view. */
    MatrixView& getMatrixView() { return *matrixView; }

    /** @return Reference to the contextual hint view. */
    HintView& getHintView() { return *hintView; }

    /** @brief The button used to open new audio files. */
    TransportButton openButton;
    /** @brief The button used to toggle between different application modes. */
    TransportButton modeButton;
    /** @brief The button used to toggle the statistics display. */
    TransportButton statsButton;
    /** @brief The button used to toggle between Mono/Stereo channel views. */
    TransportButton channelViewButton;
    /** @brief The selector for changing application themes. */
    juce::ComboBox themeSelector;
    /** @brief The button to cycle themes upwards. */
    TransportButton themeUpButton;
    /** @brief The button to cycle themes downwards. */
    TransportButton themeDownButton;

    /** @brief The modular transport strip containing Play, Stop, etc. */
    std::unique_ptr<TransportStrip> transportStrip;
    /** @brief The modular volume control view. */
    std::unique_ptr<VolumeView> volumeView;
    /** @brief The matrix view showing analysis status. */
    std::unique_ptr<MatrixView> matrixView;
    /** @brief The hint view providing contextual user feedback. */
    std::unique_ptr<HintView> hintView;

  private:
    ControlPanel &controlPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBarView)
};

#endif
