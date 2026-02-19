/**
 * @file SessionState.h
 * @brief Metadata Resource Pool and thread-safe cache access.
 * @ingroup Engine
 */

#pragma once

#include "MainDomain.h"
#include "FileMetadata.h"
#include <juce_core/juce_core.h>
#include <map>

/**
 * @class SessionState
 * @brief Metadata Resource Pool and thread-safe cache access. Home: Engine.
 *
 */
class SessionState {
public:
    /**
     * @class Listener
     * @brief Metadata Resource Pool and thread-safe cache access. Home: Engine.
     *
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        /**
         * @brief Undocumented method.
         * @param prefs [in] Description for prefs.
         */
        virtual void cutPreferenceChanged(const MainDomain::CutPreferences& prefs) = 0;
    };

    /**
     * @brief Undocumented method.
     */
    SessionState();

    /**
     * @brief Undocumented method.
     * @param listener [in] Description for listener.
     */
    void addListener(Listener* listener);
    /**
     * @brief Undocumented method.
     * @param listener [in] Description for listener.
     */
    void removeListener(Listener* listener);

    // Getters
    /**
     * @brief Gets the CutPrefs.
     * @return const MainDomain::CutPreferences&
     */
    const MainDomain::CutPreferences& getCutPrefs() const { return cutPrefs; }

    // Setters
    /**
     * @brief Sets the CutActive.
     * @param active [in] Description for active.
     */
    void setCutActive(bool active);
    /**
     * @brief Sets the AutoCutInActive.
     * @param active [in] Description for active.
     */
    void setAutoCutInActive(bool active);
    /**
     * @brief Sets the AutoCutOutActive.
     * @param active [in] Description for active.
     */
    void setAutoCutOutActive(bool active);
    /**
     * @brief Sets the ThresholdIn.
     * @param threshold [in] Description for threshold.
     */
    void setThresholdIn(float threshold);
    /**
     * @brief Sets the ThresholdOut.
     * @param threshold [in] Description for threshold.
     */
    void setThresholdOut(float threshold);
    /**
     * @brief Sets the CutIn.
     * @param value [in] Description for value.
     */
    void setCutIn(double value);
    /**
     * @brief Sets the CutOut.
     * @param value [in] Description for value.
     */
    void setCutOut(double value);
    /**
     * @brief Gets the CutIn.
     * @return double
     */
    double getCutIn() const { return getMetadataForFile(currentFilePath).cutIn; }
    /**
     * @brief Gets the CutOut.
     * @return double
     */
    double getCutOut() const { return getMetadataForFile(currentFilePath).cutOut; }
    /**
     * @brief Gets the MetadataForFile.
     * @param filePath [in] Description for filePath.
     * @return FileMetadata
     */
    FileMetadata getMetadataForFile(const juce::String& filePath) const;
    /**
     * @brief Gets the CurrentMetadata.
     * @return FileMetadata
     */
    FileMetadata getCurrentMetadata() const { return getMetadataForFile(currentFilePath); }
    /**
     * @brief Sets the MetadataForFile.
     * @param filePath [in] Description for filePath.
     * @param newMetadata [in] Description for newMetadata.
     */
    void setMetadataForFile(const juce::String& filePath, const FileMetadata& newMetadata);
    /**
     * @brief Checks if sMetadataForFile.
     * @param filePath [in] Description for filePath.
     * @return bool
     */
    bool hasMetadataForFile(const juce::String& filePath) const;
    /**
     * @brief Sets the CurrentFilePath.
     * @param filePath [in] Description for filePath.
     */
    void setCurrentFilePath(const juce::String& filePath) { currentFilePath = filePath; }
    /**
     * @brief Gets the CurrentFilePath.
     * @return const juce::String&
     */
    const juce::String& getCurrentFilePath() const { return currentFilePath; }

private:
    MainDomain::CutPreferences cutPrefs;
    juce::String currentFilePath;
    std::map<juce::String, FileMetadata> metadataCache;
    juce::ListenerList<Listener> listeners;
};
