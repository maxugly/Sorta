#pragma once

#include "MainDomain.h"
#include "FileMetadata.h"
#include <juce_core/juce_core.h>

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
    double getCutIn() const { return currentFileMetadata.cutIn; }
    double getCutOut() const { return currentFileMetadata.cutOut; }
    const FileMetadata& getCurrentMetadata() const { return currentFileMetadata; }
    void updateCurrentMetadata(const FileMetadata& data);

    bool isLooping;
    bool autoplay;

private:
    MainDomain::CutPreferences cutPrefs;
    FileMetadata currentFileMetadata;
    juce::ListenerList<Listener> listeners;
};
