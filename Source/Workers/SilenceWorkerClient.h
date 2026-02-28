#ifndef AUDIOFILER_SILENCEWORKERCLIENT_H
#define AUDIOFILER_SILENCEWORKERCLIENT_H

#if defined(JUCE_HEADLESS)
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file SilenceWorkerClient.h
 * @ingroup Logic
 * @brief Logic-tier communication interface for silence detection status reporting.
 * 
 * @details Architecturally, SilenceWorkerClient is an "Abstract Communication Interface" 
 *          that allows the SilenceAnalysisWorker (a background-threaded entity) 
 *          to push updates and query user settings without knowing the details of the 
 *          UI implementation. 
 * 
 *          This interface decouples the high-performance worker from the visual 
 *          status bar and message handling logic, ensuring that the worker 
 *          remains portable and testable.
 * 
 * @see SilenceAnalysisWorker
 * @see SilenceDetectionLogger
 */
class SilenceWorkerClient {
  public:
    virtual ~SilenceWorkerClient() = default;

    /**
     * @brief Pushes a status update or error message to the client.
     * @param message Human-readable text for the user.
     * @param isError True if the message represents a failure (e.g., file too large).
     * @note This method is typically called from a background thread and should 
     *       be treated as potentially asynchronous.
     */
    virtual void logStatusMessage(const juce::String &message, bool isError = false) = 0;

    /**
     * @brief Queries whether automatic 'In' point detection is enabled.
     * @return True if the worker should automatically update the 'In' marker.
     */
    virtual bool isAutoCutInActive() const = 0;

    /**
     * @brief Queries whether automatic 'Out' point detection is enabled.
     * @return True if the worker should automatically update the 'Out' marker.
     */
    virtual bool isAutoCutOutActive() const = 0;
};

#endif
