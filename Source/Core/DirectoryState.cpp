#include "Core/DirectoryState.h"

void DirectoryState::addListener(Listener* l) { listeners.add(l); }
void DirectoryState::removeListener(Listener* l) { listeners.remove(l); }

void DirectoryState::setDestination(int index, const juce::String& path) {
    if (index < 0 || index >= 10) return;
    const juce::ScopedLock lock(stateLock);
    if (destinations[static_cast<size_t>(index)] != path) {
        destinations[static_cast<size_t>(index)] = path;
        listeners.call([index, path](Listener& l) { l.destinationChanged(index, path); });
    }
}

juce::String DirectoryState::getDestination(int index) const {
    if (index < 0 || index >= 10) return {};
    const juce::ScopedLock lock(stateLock);
    return destinations[static_cast<size_t>(index)];
}

void DirectoryState::setCopyMode(bool shouldCopy) {
    const juce::ScopedLock lock(stateLock);
    if (isCopyMode != shouldCopy) {
        isCopyMode = shouldCopy;
        listeners.call([shouldCopy](Listener& l) { l.routingModeChanged(shouldCopy); });
    }
}

bool DirectoryState::getCopyMode() const {
    const juce::ScopedLock lock(stateLock);
    return isCopyMode;
}
