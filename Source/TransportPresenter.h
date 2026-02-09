#pragma once

#include <JuceHeader.h>

class ControlPanel;

/**
 * @class TransportPresenter
 * @brief Encapsulates the behaviour of transport-related toggles (loop, autoplay, cut mode).
 *
 * This presenter keeps ControlPanel lean by owning the button callbacks that manipulate
 * loop mode, autoplay state, and cut mode enforcement. It talks back to ControlPanel to
 * update shared state and to AudioPlayer for immediate playback adjustments.
 */
class TransportPresenter final
{
public:
    /**
     * @brief Constructs the presenter.
     * @param ownerPanel Reference to the ControlPanel that exposes the transport buttons/state.
     */
    explicit TransportPresenter(ControlPanel& ownerPanel);

    /**
     * @brief Handles the loop button toggle event.
     * @param shouldLoop True if looping should be enabled.
     */
    void handleLoopToggle(bool shouldLoop);

    /**
     * @brief Handles the autoplay button toggle event.
     * @param shouldAutoplay True if autoplay should be enabled.
     */
    void handleAutoplayToggle(bool shouldAutoplay);

    /**
     * @brief Handles the cut mode button toggle event.
     * @param enableCutMode True to enable cut mode, false to disable.
     */
    void handleCutModeToggle(bool enableCutMode);

private:
    void enforceCutLoopBounds() const;

    ControlPanel& owner;
};
