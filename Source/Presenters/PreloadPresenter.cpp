#include "Presenters/PreloadPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/FileQueueView.h"
#include "Core/AudioPlayer.h"

PreloadPresenter::PreloadPresenter(ControlPanel& ownerPanel, FileQueueView& view)
    : owner(ownerPanel),
      queueView(view),
      preloadWorker(ownerPanel.getSessionState(), ownerPanel.getAudioPlayer().getFormatManager()) {
    owner.getSessionState().addListener(this);

    FileQueueViewState initialState;
    queueView.updateState(initialState);
}

PreloadPresenter::~PreloadPresenter() {
    owner.getSessionState().removeListener(this);
    preloadWorker.clearQueue();
}

void PreloadPresenter::queueChanged(const std::vector<juce::String>& newQueue) {
    FileQueueViewState state;

    for (const auto& path : newQueue) {
        juce::File f(path);
        FileQueueItem item;
        item.filename = f.getFileName();

        // Format Size (MB/KB)
        juce::int64 sizeBytes = f.getSize();
        if (sizeBytes > 1024 * 1024) {
            item.sizeStr = juce::String(sizeBytes / (1024.0 * 1024.0), 1) + " MB";
        } else {
            item.sizeStr = juce::String(sizeBytes / 1024) + " KB";
        }

        // Format Date
        item.modifiedStr = f.getLastModificationTime().formatted("%Y-%m-%d %H:%M");

        state.upcomingFiles.push_back(item);
    }
    queueView.updateState(state);

    preloadWorker.clearQueue();
    for (const auto& path : newQueue) {
        preloadWorker.enqueueFile(path);
    }
}
