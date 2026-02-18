#pragma once

#include "MainDomain.h"
#include "FileMetadata.h"
#include <juce_core/juce_core.h>
#include <map>

class SessionState {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) = 0;
    };

    SessionState();

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    // Getters
    const MainDomain::CutPreferences& getCutPrefs() const { return cutPrefs; }

    // Setters
    void setCutActive(bool active);
    void setAutoCutInActive(bool active);
    void setAutoCutOutActive(bool active);
    void setThresholdIn(float threshold);
    void setThresholdOut(float threshold);
    void setCutIn(double value);
    void setCutOut(double value);
    double getCutIn() const { return getMetadataForFile(currentFilePath).cutIn; }
    double getCutOut() const { return getMetadataForFile(currentFilePath).cutOut; }
    FileMetadata getMetadataForFile(const juce::String& filePath) const;
    FileMetadata getCurrentMetadata() const { return getMetadataForFile(currentFilePath); }
    void setMetadataForFile(const juce::String& filePath, const FileMetadata& newMetadata);
    bool hasMetadataForFile(const juce::String& filePath) const;
    void setCurrentFilePath(const juce::String& filePath) { currentFilePath = filePath; }
    const juce::String& getCurrentFilePath() const { return currentFilePath; }

private:
    MainDomain::CutPreferences cutPrefs;
    juce::String currentFilePath;
    std::map<juce::String, FileMetadata> metadataCache;
    juce::ListenerList<Listener> listeners;
};
