#pragma once

#include "Core/FileMetadata.h"
#include "Core/AppEnums.h"
#include "MainDomain.h"
#include <juce_core/juce_core.h>
#include <map>

/**
 * @file SessionState.h
 * @ingroup State
 * @brief The central data model and state coordinator for the Audiofiler application.
 * 
 * @details Known colloquially as "The Brain," SessionState acts as the single source of truth 
 *          for all application-wide settings, user preferences, and metadata. It follows 
 *          the "Radio Station" broadcast pattern: any component (the "Listeners") can 
 *          tune in to specific state changes without needing a direct reference to the 
 *          components that initiated those changes.
 * 
 *          Architecturally, this class implements the Model in the Model-View-Presenter (MVP) 
 *          pattern. It provides a thread-safe environment for state access via internal 
 *          CriticalSections, ensuring that background workers (like SilenceAnalysisWorker) 
 *          and the UI thread (via Presenters) can interact with the state without race conditions.
 * 
 *          It manages:
 *          - File Metadata Cache: Persistent storage for audio file analysis.
 *          - Cut Preferences: User-defined thresholds and lock states.
 *          - Playback State: Master volume and view configurations.
 *          - Navigation State: Zoom levels and focus paths.
 * 
 * @see AudioPlayer
 * @see ControlPanel
 * @see FileMetadata
 * @see PresenterCore
 */
class SessionState {
  public:
    /**
     * @brief Interface for any object that needs to react to SessionState changes.
     * @details Presenters typically implement this interface to update their respective Views 
     *          when the underlying Model changes. This ensures a clean separation of concerns 
     *          and prevents circular dependencies.
     */
    class Listener {
      public:
        virtual ~Listener() = default;

        /**
         * @brief Called when the primary active audio file path changes.
         * @param filePath The absolute path to the new audio file.
         */
        virtual void fileChanged(const juce::String &filePath) {
            juce::ignoreUnused(filePath);
        }

        /**
         * @brief Called when global cut preferences (thresholds, locks) are modified.
         * @param prefs The updated CutPreferences structure.
         */
        virtual void cutPreferenceChanged(const MainDomain::CutPreferences &prefs) {
            juce::ignoreUnused(prefs);
        }

        /**
         * @brief Called when the specific 'In' boundary point of a cut is moved.
         * @param value The new position in seconds.
         */
        virtual void cutInChanged(double value) {
            juce::ignoreUnused(value);
        }

        /**
         * @brief Called when the specific 'Out' boundary point of a cut is moved.
         * @param value The new position in seconds.
         */
        virtual void cutOutChanged(double value) {
            juce::ignoreUnused(value);
        }

        /**
         * @brief Called when the master playback volume is adjusted.
         * @param newVolume The new volume level (0.0 to 1.0).
         */
        virtual void volumeChanged(float newVolume) {
            juce::ignoreUnused(newVolume);
        }

        /**
         * @brief Called when the UI switches between different visualization modes.
         * @param newMode The new ViewMode (Classic, Waveform, etc.).
         */
        virtual void viewModeChanged(AppEnums::ViewMode newMode) {
            juce::ignoreUnused(newMode);
        }

        /**
         * @brief Called when the waveform display changes between Mono/Stereo/etc.
         * @param newMode The new ChannelViewMode.
         */
        virtual void channelViewModeChanged(AppEnums::ChannelViewMode newMode) {
            juce::ignoreUnused(newMode);
        }
    };

    /**
     * @brief Initializes the SessionState with default values.
     * @details Default values are typically loaded from persistent storage via Config utilities 
     *          shortly after construction.
     */
    SessionState();

    /**
     * @brief Registers a listener to receive state change broadcasts.
     * @param listener Pointer to the listener implementation.
     */
    void addListener(Listener *listener);

    /**
     * @brief Unregisters a listener to stop receiving broadcasts.
     * @param listener Pointer to the listener to remove.
     */
    void removeListener(Listener *listener);

    /**
     * @brief Retrieves the current global cut preferences.
     * @return A copy of the current CutPreferences structure.
     */
    MainDomain::CutPreferences getCutPrefs() const;

    /**
     * @brief Updates the master volume level.
     * @param v The new volume (0.0 to 1.0).
     * @note This triggers a broadcast to the AudioPlayer and VolumePresenter.
     */
    void setVolume(float v);

    /**
     * @brief Retrieves the current master volume level.
     * @return Float value between 0.0 and 1.0.
     */
    float getVolume() const;

    /**
     * @brief Toggles whether the 'Cut' functionality is active.
     * @param active True if cutting logic should be applied.
     */
    void setCutActive(bool active);

    /**
     * @brief Toggles automatic playback upon file load.
     * @param active True to enable auto-play.
     */
    void setAutoPlayActive(bool active);

    /**
     * @brief Toggles automatic 'In' boundary detection based on silence analysis.
     * @param active True to enable auto-cut-in.
     */
    void setAutoCutInActive(bool active);

    /**
     * @brief Toggles automatic 'Out' boundary detection based on silence analysis.
     * @param active True to enable auto-cut-out.
     */
    void setAutoCutOutActive(bool active);

    /**
     * @brief Locks or unlocks the 'In' boundary point.
     * @param locked True to prevent manual modification of the 'In' point.
     */
    void setInLocked(bool locked);

    /**
     * @brief Locks or unlocks the 'Out' boundary point.
     * @param locked True to prevent manual modification of the 'Out' point.
     */
    void setOutLocked(bool locked);

    /**
     * @brief Locks the length between In and Out points.
     * @param locked True to preserve the duration while moving boundaries.
     */
    void setLengthLocked(bool locked);

    /**
     * @brief Sets the dB threshold for automatic 'In' point detection.
     * @param threshold Value in decibels.
     */
    void setThresholdIn(float threshold);

    /**
     * @brief Sets the dB threshold for automatic 'Out' point detection.
     * @param threshold Value in decibels.
     */
    void setThresholdOut(float threshold);

    /**
     * @brief Manually sets the 'In' boundary point.
     * @param value Position in seconds.
     */
    void setCutIn(double value);

    /**
     * @brief Manually sets the 'Out' boundary point.
     * @param value Position in seconds.
     */
    void setCutOut(double value);

    /**
     * @brief Gets the current 'In' boundary point.
     * @return Position in seconds.
     */
    double getCutIn() const;

    /**
     * @brief Gets the current 'Out' boundary point.
     * @return Position in seconds.
     */
    double getCutOut() const;

    /**
     * @brief Updates the total duration of the currently loaded audio file.
     * @param duration Length in seconds.
     */
    void setTotalDuration(double duration);

    /**
     * @brief Gets the total duration of the current file.
     * @return Length in seconds.
     */
    double getTotalDuration() const;

    /**
     * @brief Retrieves analysis metadata for a specific file path.
     * @param filePath Absolute path to the audio file.
     * @return FileMetadata structure containing detected silence regions.
     */
    FileMetadata getMetadataForFile(const juce::String &filePath) const;

    /**
     * @brief Retrieves metadata for the currently active file.
     * @return FileMetadata structure.
     */
    FileMetadata getCurrentMetadata() const;

    /**
     * @brief Stores analysis metadata for a specific file.
     * @param filePath Absolute path to the audio file.
     * @param newMetadata The metadata object to cache.
     */
    void setMetadataForFile(const juce::String &filePath, const FileMetadata &newMetadata);

    /**
     * @brief Checks if analysis metadata exists for the given file.
     * @param filePath Path to check.
     * @return True if metadata is cached.
     */
    bool hasMetadataForFile(const juce::String &filePath) const;

    /**
     * @brief Changes the active file path and resets local state as needed.
     * @param filePath Absolute path to the new file.
     * @note This triggers a cascade of updates across the entire application.
     */
    void setCurrentFilePath(const juce::String &filePath);

    /**
     * @brief Retrieves the path to the currently loaded file.
     * @return Absolute file path string.
     */
    juce::String getCurrentFilePath() const;

    /**
     * @brief Gets the current horizontal zoom factor for the waveform display.
     * @return Zoom factor where 1.0 is full view.
     */
    float getZoomFactor() const { return m_zoomFactor; }

    /**
     * @brief Sets the horizontal zoom factor.
     * @param factor Zoom value, clamped between 1.0 and 1,000,000.
     */
    void setZoomFactor(float factor) {
        m_zoomFactor = juce::jlimit(1.0f, 1000000.0f, factor);
    }

    /**
     * @brief Gets the current UI view mode.
     * @return AppEnums::ViewMode.
     */
    AppEnums::ViewMode getViewMode() const { return currentMode; }

    /**
     * @brief Sets the UI view mode.
     * @param mode New ViewMode to apply.
     */
    void setViewMode(AppEnums::ViewMode mode);

    /**
     * @brief Gets the current channel visualization mode.
     * @return AppEnums::ChannelViewMode.
     */
    AppEnums::ChannelViewMode getChannelViewMode() const { return currentChannelViewMode; }

    /**
     * @brief Sets the channel visualization mode.
     * @param mode New ChannelViewMode (Mono, Stereo, etc.).
     */
    void setChannelViewMode(AppEnums::ChannelViewMode mode);

  private:
    MainDomain::CutPreferences cutPrefs;                /**< Current user preferences for the cutting engine. */
    juce::String currentFilePath;                        /**< Path to the currently loaded audio asset. */
    double totalDuration{0.0};                         /**< Duration of the current file in seconds. */
    std::map<juce::String, FileMetadata> metadataCache; /**< In-memory cache of analyzed file metadata. */
    juce::ListenerList<Listener> listeners;             /**< List of objects observing this state. */

    float m_zoomFactor{10.0f};                          /**< Current waveform zoom level. */
    float m_masterVolume{1.0f};                        /**< Master gain level (0.0 to 1.0). */
    AppEnums::ViewMode currentMode{AppEnums::ViewMode::Classic}; /**< Active layout mode for the UI. */
    AppEnums::ChannelViewMode currentChannelViewMode{AppEnums::ChannelViewMode::Mono}; /**< Active channel rendering mode. */

    mutable juce::CriticalSection stateLock;            /**< Mutex protecting multi-threaded access to state data. */
};
