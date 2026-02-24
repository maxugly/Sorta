

#include "Core/SessionState.h"

SessionState::SessionState() {
    cutPrefs.cutIn = 0.0;
    cutPrefs.cutOut = 0.0;
}

void SessionState::addListener(Listener *listener) {
    listeners.add(listener);
}

void SessionState::removeListener(Listener *listener) {
    listeners.remove(listener);
}

MainDomain::CutPreferences SessionState::getCutPrefs() const {
    const juce::ScopedLock lock(stateLock);
    return cutPrefs;
}

void SessionState::setVolume(float v) {
    const juce::ScopedLock lock(stateLock);
    const float clampedV = juce::jlimit(0.0f, 1.0f, v);
    if (m_masterVolume != clampedV) {
        m_masterVolume = clampedV;
        listeners.call([clampedV](Listener &l) { l.volumeChanged(clampedV); });
    }
}

float SessionState::getVolume() const {
    const juce::ScopedLock lock(stateLock);
    return m_masterVolume;
}

void SessionState::setCutActive(bool active) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.active != active) {
        cutPrefs.active = active;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setAutoPlayActive(bool active) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.autoplay != active) {
        cutPrefs.autoplay = active;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setAutoCutInActive(bool active) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.autoCut.inActive != active) {
        cutPrefs.autoCut.inActive = active;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setAutoCutOutActive(bool active) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.autoCut.outActive != active) {
        cutPrefs.autoCut.outActive = active;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setLengthLocked(bool locked) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.lengthLocked != locked) {
        cutPrefs.lengthLocked = locked;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setInLocked(bool locked) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.inLocked != locked) {
        cutPrefs.inLocked = locked;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setOutLocked(bool locked) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.outLocked != locked) {
        cutPrefs.outLocked = locked;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setThresholdIn(float threshold) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.autoCut.thresholdIn != threshold) {
        cutPrefs.autoCut.thresholdIn = threshold;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setThresholdOut(float threshold) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.autoCut.thresholdOut != threshold) {
        cutPrefs.autoCut.thresholdOut = threshold;
        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setCutIn(double value) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.inLocked) return;
    double clampedValue = juce::jlimit(0.0, totalDuration, value);

    if (cutPrefs.lengthLocked) {
        const double length = cutPrefs.cutOut - cutPrefs.cutIn;
        clampedValue = juce::jlimit(0.0, totalDuration - length, clampedValue);
        cutPrefs.cutOut = clampedValue + length;
        cutPrefs.cutIn = clampedValue;
        
        if (!currentFilePath.isEmpty()) {
            metadataCache[currentFilePath].cutIn = cutPrefs.cutIn;
            metadataCache[currentFilePath].cutOut = cutPrefs.cutOut;
        }

        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
        listeners.call([this](Listener &l) { l.cutInChanged(cutPrefs.cutIn); });
        listeners.call([this](Listener &l) { l.cutOutChanged(cutPrefs.cutOut); });
    } else {
        clampedValue = juce::jmin(clampedValue, cutPrefs.cutOut);
        if (cutPrefs.cutIn != clampedValue) {
            cutPrefs.cutIn = clampedValue;
            if (!currentFilePath.isEmpty())
                metadataCache[currentFilePath].cutIn = clampedValue;
            listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
            listeners.call([clampedValue](Listener &l) { l.cutInChanged(clampedValue); });
        }
    }
}

void SessionState::setCutOut(double value) {
    const juce::ScopedLock lock(stateLock);
    if (cutPrefs.outLocked) return;
    double clampedValue = juce::jlimit(0.0, totalDuration, value);

    if (cutPrefs.lengthLocked) {
        const double length = cutPrefs.cutOut - cutPrefs.cutIn;
        clampedValue = juce::jlimit(length, totalDuration, clampedValue);
        cutPrefs.cutIn = clampedValue - length;
        cutPrefs.cutOut = clampedValue;

        if (!currentFilePath.isEmpty()) {
            metadataCache[currentFilePath].cutIn = cutPrefs.cutIn;
            metadataCache[currentFilePath].cutOut = cutPrefs.cutOut;
        }

        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
        listeners.call([this](Listener &l) { l.cutInChanged(cutPrefs.cutIn); });
        listeners.call([this](Listener &l) { l.cutOutChanged(cutPrefs.cutOut); });
    } else {
        clampedValue = juce::jmax(clampedValue, cutPrefs.cutIn);
        if (cutPrefs.cutOut != clampedValue) {
            cutPrefs.cutOut = clampedValue;
            if (!currentFilePath.isEmpty())
                metadataCache[currentFilePath].cutOut = clampedValue;
            listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
            listeners.call([clampedValue](Listener &l) { l.cutOutChanged(clampedValue); });
        }
    }
}

double SessionState::getCutIn() const {
    const juce::ScopedLock lock(stateLock);
    return cutPrefs.cutIn;
}

double SessionState::getCutOut() const {
    const juce::ScopedLock lock(stateLock);
    return cutPrefs.cutOut;
}

void SessionState::setTotalDuration(double duration) {
    const juce::ScopedLock lock(stateLock);
    totalDuration = duration;
}

double SessionState::getTotalDuration() const {
    const juce::ScopedLock lock(stateLock);
    return totalDuration;
}

FileMetadata SessionState::getMetadataForFile(const juce::String &filePath) const {
    const juce::ScopedLock lock(stateLock);
    const auto it = metadataCache.find(filePath);
    if (it != metadataCache.end())
        return it->second;
    return FileMetadata{};
}

FileMetadata SessionState::getCurrentMetadata() const {
    const juce::ScopedLock lock(stateLock);
    return getMetadataForFile(currentFilePath);
}

bool SessionState::hasMetadataForFile(const juce::String &filePath) const {
    const juce::ScopedLock lock(stateLock);
    return metadataCache.find(filePath) != metadataCache.end();
}

void SessionState::setCurrentFilePath(const juce::String &filePath) {
    const juce::ScopedLock lock(stateLock);
    if (currentFilePath != filePath) {
        currentFilePath = filePath;

        // Sync cutPrefs from metadata cache for the new current file
        const auto it = metadataCache.find(filePath);
        if (it != metadataCache.end()) {
            const auto &metadata = it->second;
            const double inVal = juce::jlimit(0.0, totalDuration, metadata.cutIn);
            const double outVal = juce::jlimit(0.0, totalDuration, metadata.cutOut);

            cutPrefs.cutIn = juce::jmin(inVal, outVal);
            cutPrefs.cutOut = juce::jmax(inVal, outVal);

            listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
            listeners.call([this](Listener &l) { l.cutInChanged(cutPrefs.cutIn); });
            listeners.call([this](Listener &l) { l.cutOutChanged(cutPrefs.cutOut); });
        }

        listeners.call([filePath](Listener &l) { l.fileChanged(filePath); });
    }
}

juce::String SessionState::getCurrentFilePath() const {
    const juce::ScopedLock lock(stateLock);
    return currentFilePath;
}

void SessionState::setMetadataForFile(const juce::String &filePath,
                                      const FileMetadata &newMetadata) {
    const juce::ScopedLock lock(stateLock);
    metadataCache[filePath] = newMetadata;

    if (filePath == currentFilePath) {
        // Apply clamping when syncing to active cutPrefs
        const double inVal = juce::jlimit(0.0, totalDuration, newMetadata.cutIn);
        const double outVal = juce::jlimit(0.0, totalDuration, newMetadata.cutOut);

        // Ensure CutIn <= Out
        cutPrefs.cutIn = juce::jmin(inVal, outVal);
        cutPrefs.cutOut = juce::jmax(inVal, outVal);

        listeners.call([this](Listener &l) { l.cutPreferenceChanged(cutPrefs); });
        listeners.call([this](Listener &l) { l.cutInChanged(cutPrefs.cutIn); });
        listeners.call([this](Listener &l) { l.cutOutChanged(cutPrefs.cutOut); });
    }
}

void SessionState::setViewMode(AppEnums::ViewMode mode) {
    const juce::ScopedLock lock(stateLock);
    if (currentMode != mode) {
        currentMode = mode;
        listeners.call([mode](Listener &l) { l.viewModeChanged(mode); });
    }
}

void SessionState::setChannelViewMode(AppEnums::ChannelViewMode mode) {
    const juce::ScopedLock lock(stateLock);
    if (currentChannelViewMode != mode) {
        currentChannelViewMode = mode;
        listeners.call([mode](Listener &l) { l.channelViewModeChanged(mode); });
    }
}
