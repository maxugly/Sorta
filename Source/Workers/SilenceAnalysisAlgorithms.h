#ifndef SILENCE_ANALYSIS_ALGORITHMS_H
#define SILENCE_ANALYSIS_ALGORITHMS_H

#ifdef JUCE_HEADLESS
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>
#else
#include <JuceHeader.h>
#endif

/**
 * @file SilenceAnalysisAlgorithms.h
 * @ingroup AudioEngine
 * @brief Static utility class for low-level audio sample analysis and silence detection.
 * 
 * @details Architecturally, SilenceAnalysisAlgorithms serves as a "Pure Logic Engine" 
 *          that contains the mathematical core of the application's silence-stripping 
 *          capability. It is designed to be completely stateless, operating directly 
 *          on `juce::AudioFormatReader` streams.
 * 
 *          The class implements an optimized chunk-based scanning strategy:
 *          - **Memory Efficiency**: Instead of loading whole files into RAM, it 
 *            processes audio in fixed-size buffers (e.g., 65,536 samples), making 
 *            it safe for multi-gigabyte recordings.
 *          - **Multichannel Awareness**: It scans all available channels 
 *            independently, using a "Logical OR" approach—if any channel exceeds 
 *             the threshold, the region is considered non-silent.
 *          - **Responsiveness**: It supports cancellation via `juce::Thread` tokens, 
 *            allowing the `SilenceAnalysisWorker` to abort long scans instantly 
 *            if the user changes files or settings.
 * 
 * @see SilenceAnalysisWorker
 * @see AudioFormatReader
 */
class SilenceAnalysisAlgorithms {
  public:
    /**
     * @brief Identifies the first non-silent sample from the start of an audio stream.
     * @details This function performs a forward linear scan. It calculates the absolute 
     *          peak amplitude of each chunk.
     * 
     *          Mathematical approach:
     *          1. Seek to the start of the file.
     *          2. Read a block of N samples across all channels.
     *          3. For each sample s: if abs(s) > threshold, return current_sample_index.
     *          4. Repeat until EOF or thread cancellation.
     *
     * @param reader The audio reader providing the sample stream.
     * @param threshold The amplitude threshold (linear range 0.0 to 1.0).
     * @param thread Optional pointer to the calling thread for exit signal polling.
     * @return The absolute sample index of the first non-silent point, or 0 if 
     *         the entire file is below the threshold.
     */
    static juce::int64 findSilenceIn(juce::AudioFormatReader &reader, float threshold,
                                     juce::Thread *thread = nullptr);

    /**
     * @brief Identifies the last non-silent sample by scanning backwards from the EOF.
     * @details This function performs a reverse linear scan, which is significantly 
     *          more efficient for finding the "Out" point in long files with trailing silence.
     * 
     *          Mathematical approach:
     *          1. Calculate the final sample index based on reader length.
     *          2. Read a block of N samples ending at the current cursor.
     *          3. Scan the block *backwards*: if abs(s) > threshold, return current_sample_index.
     *          4. Move the cursor back by N samples and repeat until start-of-file.
     *
     * @param reader The audio reader providing the sample stream.
     * @param threshold The amplitude threshold (linear range 0.0 to 1.0).
     * @param thread Optional pointer to the calling thread for exit signal polling.
     * @return The absolute sample index of the last non-silent point, or the 
     *         total file length if no silence is found.
     */
    static juce::int64 findSilenceOut(juce::AudioFormatReader &reader, float threshold,
                                      juce::Thread *thread = nullptr);
};

#endif
