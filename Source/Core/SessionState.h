#pragma once

#include "Core/FileMetadata.h"
#include "Core/AppEnums.h"
#include "MainDomain.h"
#include <juce_core/juce_core.h>
#include <map>

/**
 * @file SessionState.h
 * @ingroup State
 * @brief The central data model for the application.
 * @details This class holds the current application state, including file metadata,
 *          cut preferences, and other user settings. It acts as a "hub" for communication,
 *          allowing components to listen for state changes without tight coupling.
 *
 *          It uses `juce::ListenerList` to notify registered listeners when properties change.
 *
 * @see AudioPlayer
 * @see ControlPanel
 * @see FileMetadata
 */
class SessionState {
  public:
    class Listener {
      public:
        virtual ~Listener() = default;

        virtual void fileChanged(const juce::String &filePath) {
            juce::ignoreUnused(filePath);
        }
        virtual void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
            juce::ignoreUnused(prefs);
        }
        virtual void cutInChanged(double value) {
            juce::ignoreUnused(value);
        }
        virtual void cutOutChanged(double value) {
            juce::ignoreUnused(value);
        }
        virtual void volumeChanged(float newVolume) {
            juce::ignoreUnused(newVolume);
        }
        virtual void viewModeChanged(AppEnums::ViewMode newMode) {
            juce::ignoreUnused(newMode);
        }
        virtual void channelViewModeChanged(AppEnums::ChannelViewMode newMode) {
            juce::ignoreUnused(newMode);
        }
    };

    SessionState();

    void addListener(Listener *listener);

    void removeListener(Listener *listener);

    MainDomain::CutPreferences getCutPrefs() const;

    void setVolume(float v);
    float getVolume() const;

    void setCutActive(bool active);

    void setAutoPlayActive(bool active);

    void setAutoCutInActive(bool active);

    void setAutoCutOutActive(bool active);

    void setThresholdIn(float threshold);

    void setThresholdOut(float threshold);

    void setCutIn(double value);

    void setCutOut(double value);
    double getCutIn() const;
    double getCutOut() const;

    void setTotalDuration(double duration);
    double getTotalDuration() const;

    FileMetadata getMetadataForFile(const juce::String &filePath) const;
    FileMetadata getCurrentMetadata() const;
    void setMetadataForFile(const juce::String &filePath, const FileMetadata &newMetadata);

    bool hasMetadataForFile(const juce::String &filePath) const;
    void setCurrentFilePath(const juce::String &filePath);
    juce::String getCurrentFilePath() const;

    float getZoomFactor() const { return m_zoomFactor; }
    void setZoomFactor(float factor) {
        m_zoomFactor = juce::jlimit(1.0f, 1000000.0f, factor);
    }

    AppEnums::ViewMode getViewMode() const { return currentMode; }
    void setViewMode(AppEnums::ViewMode mode);

    AppEnums::ChannelViewMode getChannelViewMode() const { return currentChannelViewMode; }
    void setChannelViewMode(AppEnums::ChannelViewMode mode);

  private:
    MainDomain::CutPreferences cutPrefs;
    juce::String currentFilePath;
    double totalDuration{0.0};
    std::map<juce::String, FileMetadata> metadataCache;
    juce::ListenerList<Listener> listeners;

    float m_zoomFactor{10.0f};
    float m_masterVolume{1.0f};
    AppEnums::ViewMode currentMode{AppEnums::ViewMode::Classic};
    AppEnums::ChannelViewMode currentChannelViewMode{AppEnums::ChannelViewMode::Mono};

    mutable juce::CriticalSection stateLock;
};
