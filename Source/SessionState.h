

#pragma once

#include "MainDomain.h"
#include "FileMetadata.h"
#include <juce_core/juce_core.h>
#include <map>

/**
 * @ingroup State
 * @class SessionState
 * @brief The central data model and event broadcaster.
 * @details Stores the current file path, cut preferences (in/out points), and file metadata.
 * Components can register as listeners to be notified of state changes.
 */
class SessionState {
public:

    /**
     * @brief Interface for receiving state change notifications.
     * @details Implement this interface to react to changes in cut preferences or file metadata.
     */
    class Listener {
    public:
        virtual ~Listener() = default;

        virtual void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) = 0;
    };

    SessionState();

    void addListener(Listener* listener);

    void removeListener(Listener* listener);

    MainDomain::CutPreferences getCutPrefs() const;

    void setCutActive(bool active);

    void setAutoCutInActive(bool active);

    void setAutoCutOutActive(bool active);

    void setThresholdIn(float threshold);

    void setThresholdOut(float threshold);

    void setCutIn(double value);

    void setCutOut(double value);
    double getCutIn() const;
    double getCutOut() const;
    FileMetadata getMetadataForFile(const juce::String& filePath) const;
    FileMetadata getCurrentMetadata() const;
    void setMetadataForFile(const juce::String& filePath, const FileMetadata& newMetadata);

    bool hasMetadataForFile(const juce::String& filePath) const;
    void setCurrentFilePath(const juce::String& filePath);
    juce::String getCurrentFilePath() const;

private:
    /** @brief Stores the current cut in/out points and enabled states. */
    MainDomain::CutPreferences cutPrefs;
    juce::String currentFilePath;
    /** @brief Caches metadata (e.g., previous cut points) for previously loaded files. */
    std::map<juce::String, FileMetadata> metadataCache;
    juce::ListenerList<Listener> listeners;

    mutable juce::CriticalSection stateLock;
};
