#pragma once
#include <JuceHeader.h>
#include "Core/DirectoryState.h"
#include "Workers/RoutingWorker.h"

class SessionState;
class AudioPlayer;

/**
 * @file DirectoryPresenter.h
 * @ingroup Logic
 * @brief Presenter Glue for executing file routing commands and advancing the queue.
 */
class DirectoryPresenter final {
public:
    DirectoryPresenter(SessionState& sessionState,
                       AudioPlayer& audioPlayer,
                       DirectoryState& directoryState,
                       RoutingWorker& routingWorker);
    ~DirectoryPresenter() = default;

    /**
     * @brief Routes the current file to the specified destination slot (0-9).
     * Automatically advances to the next file in the queue.
     */
    void routeToDestination(int slotIndex);

private:
    SessionState& session;
    AudioPlayer& player;
    DirectoryState& dirState;
    RoutingWorker& worker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryPresenter)
};
