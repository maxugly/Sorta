#pragma once

#include <juce_core/juce_core.h>

/**
 * @file FileMetadata.h
 * @ingroup State
 * @brief Simple data container for persistent audio file analysis results.
 * 
 * @details Architecturally, FileMetadata serves as a "Passive Data Model" that 
 *          encapsulates the results of a single file analysis pass. It is 
 *          primarily used by the SessionState cache to ensure that once a file 
 *          has been scanned for silence, its results are instantly recallable 
 *          upon re-loading the file, eliminating the need for redundant CPU-heavy 
 *          background analysis.
 */
struct FileMetadata {
    /** @brief The detected or user-assigned 'In' point in seconds. */
    double cutIn{0.0};

    /** @brief The detected or user-assigned 'Out' point in seconds. */
    double cutOut{0.0};

    /** @brief True if the file has undergone a complete silence analysis pass. */
    bool isAnalyzed{false};

    /** @brief Unique hash (e.g., MD5) used to verify file identity across sessions. */
    juce::String hash;
};
