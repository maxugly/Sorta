#include "SessionState.h"

SessionState::SessionState()
{
    // Initialize cutIn and cutOut to 0.0
    cutPrefs.cutIn = 0.0;
    cutPrefs.cutOut = 0.0;
}

void SessionState::addListener(Listener* listener)
{
    listeners.add(listener);
}

void SessionState::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

void SessionState::setCutActive(bool active)
{
    if (cutPrefs.active != active)
    {
        cutPrefs.active = active;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setAutoCutInActive(bool active)
{
    if (cutPrefs.autoCut.inActive != active)
    {
        cutPrefs.autoCut.inActive = active;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setAutoCutOutActive(bool active)
{
    if (cutPrefs.autoCut.outActive != active)
    {
        cutPrefs.autoCut.outActive = active;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setThresholdIn(float threshold)
{
    if (cutPrefs.autoCut.thresholdIn != threshold)
    {
        cutPrefs.autoCut.thresholdIn = threshold;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setThresholdOut(float threshold)
{
    if (cutPrefs.autoCut.thresholdOut != threshold)
    {
        cutPrefs.autoCut.thresholdOut = threshold;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setCutIn(double value)
{
    if (cutPrefs.cutIn != value)
    {
        cutPrefs.cutIn = value;
        if (!currentFilePath.isEmpty())
            metadataCache[currentFilePath].cutIn = value;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

void SessionState::setCutOut(double value)
{
    if (cutPrefs.cutOut != value)
    {
        cutPrefs.cutOut = value;
        if (!currentFilePath.isEmpty())
            metadataCache[currentFilePath].cutOut = value;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}

FileMetadata SessionState::getMetadataForFile(const juce::String& filePath) const
{
    const auto it = metadataCache.find(filePath);
    if (it != metadataCache.end())
        return it->second;
    return FileMetadata{};
}

bool SessionState::hasMetadataForFile(const juce::String& filePath) const
{
    return metadataCache.find(filePath) != metadataCache.end();
}

void SessionState::setMetadataForFile(const juce::String& filePath, const FileMetadata& newMetadata)
{
    metadataCache[filePath] = newMetadata;

    if (filePath == currentFilePath)
    {
        cutPrefs.cutIn = newMetadata.cutIn;
        cutPrefs.cutOut = newMetadata.cutOut;
        listeners.call([this](Listener& l) { l.cutPreferenceChanged(cutPrefs); });
    }
}
