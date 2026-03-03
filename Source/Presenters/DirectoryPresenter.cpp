#include "Presenters/DirectoryPresenter.h"
#include "Core/SessionState.h"
#include "Core/AudioPlayer.h"

DirectoryPresenter::DirectoryPresenter(SessionState& sessionState,
                                       AudioPlayer& audioPlayer,
                                       DirectoryState& directoryState,
                                       RoutingWorker& routingWorker,
                                       DirectoryRoutingView& routingViewIn)
    : session(sessionState),
      player(audioPlayer),
      dirState(directoryState),
      worker(routingWorker),
      routingView(routingViewIn) {
    dirState.addListener(this);
    DirectoryRoutingViewState state;
    for (int i = 0; i < 10; ++i)
        state.destinations[i] = dirState.getDestination(i);
    routingView.updateState(state);
}

DirectoryPresenter::~DirectoryPresenter() {
    dirState.removeListener(this);
}

void DirectoryPresenter::destinationChanged(int index, const juce::String& path) {
    juce::ignoreUnused(index, path);
    DirectoryRoutingViewState state;
    for (int i = 0; i < 10; ++i)
        state.destinations[i] = dirState.getDestination(i);
    routingView.updateState(state);
}

void DirectoryPresenter::routeToDestination(int slotIndex) {
    if (slotIndex < 0 || slotIndex > 9) return;

    juce::String destPath = dirState.getDestination(slotIndex);
    if (destPath.isEmpty()) return; // Destination not set

    juce::File destDir(destPath);
    juce::File currentFile = player.getLoadedFile(); // Assuming this getter exists in AudioPlayer

    if (!currentFile.existsAsFile()) return;

    // 1. Dispatch the background file operation
    worker.enqueueJob(currentFile, destDir, dirState.getCopyMode());

    DirectoryRoutingViewState state;
    for (int i = 0; i < 10; ++i)
        state.destinations[i] = dirState.getDestination(i);
    state.lastRoutedIndex = slotIndex;
    routingView.updateState(state);
    juce::Timer::callAfterDelay(200, [this]() { clearHighlight(); });

    // 2. Advance the queue
    auto currentQueue = session.getFileQueue();
    if (!currentQueue.empty()) {
        juce::String nextFilePath = currentQueue.front();

        // Remove from queue
        currentQueue.erase(currentQueue.begin());
        session.setFileQueue(currentQueue);

        // Load next file instantly
        player.loadFile(juce::File(nextFilePath));
    }
}

void DirectoryPresenter::clearHighlight() {
    DirectoryRoutingViewState state;
    for (int i = 0; i < 10; ++i)
        state.destinations[i] = dirState.getDestination(i);
    state.lastRoutedIndex = -1;
    routingView.updateState(state);
}
