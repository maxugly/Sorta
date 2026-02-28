#include "Workers/SilenceAnalysisAlgorithms.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
/** @brief The optimal buffer size for sequential disk access and cache locality. 
 *  @details We use a fixed 64k chunk size to ensure that even multi-gigabyte files 
 *           can be scanned without ballooning RAM usage or risking integer overflows 
 *           during large-scale buffer allocations.
 */
constexpr int kChunkSize = 65536;

constexpr int kMaxChannels = 128;
} // namespace

/**
 * @details This method implements a forward-scanning linear search to identify the 
 *          first sample that exceeds the user-defined amplitude threshold. 
 *          
 *          Architecturally, it employs a "Windowed Streaming" approach:
 *          1. **Chunk-Based Processing**: Instead of loading the entire file, we 
 *             stream it in fixed 64k segments (kChunkSize). This prevents 
 *             out-of-memory crashes on extremely long audio recordings.
 *          2. **Interleaved Channel Scanning**: To ensure accuracy, we check every 
 *             channel (Mono, Stereo, or Multi-channel) at each sample index. If 
 *             ANY channel exceeds the threshold, that sample is marked as the 'In' point.
 *          3. **Thread Safety**: We periodically poll for `threadShouldExit` to 
 *             ensure that long-running analysis passes can be safely cancelled by 
 *             the user without hanging the application.
 */
juce::int64 SilenceAnalysisAlgorithms::findSilenceIn(juce::AudioFormatReader &reader,
                                                     float threshold, juce::Thread *thread) {
    const juce::int64 lengthInSamples = reader.lengthInSamples;

    if (reader.numChannels <= 0 || reader.numChannels > kMaxChannels)
        return -1;

    juce::AudioBuffer<float> buffer(reader.numChannels, kChunkSize);

    juce::int64 currentPos = 0;
    while (currentPos < lengthInSamples) {
        // Calculate the exact number of samples to read, accounting for the file's end
        const int numThisTime =
            (int)std::min((juce::int64)kChunkSize, lengthInSamples - currentPos);
        
        buffer.clear();
        if (!reader.read(&buffer, 0, numThisTime, currentPos, true, true))
            return -1;

        if (thread != nullptr) {
            if (thread->threadShouldExit())
                return -1;
            thread->wait(1);
        }

        // Perform the peak-detection pass on the current chunk
        for (int sample = 0; sample < numThisTime; ++sample) {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                // Use absolute value to catch both positive and negative peak excursions
                if (std::abs(buffer.getSample(channel, sample)) > threshold) {
                    return currentPos + sample;
                }
            }
        }
        // Advance the read pointer to the start of the next 64k window
        currentPos += numThisTime;
    }
    return -1;
}

/**
 * @details This method implements a "Reverse Crawl" algorithm to find the last 
 *          audible sample in the file (the 'Out' point).
 *          
 *          Key Algorithmic Decisions:
 *          1. **Reverse Chunking**: We start from the end of the file (`lengthInSamples`) 
 *             and work backwards in 64k chunks. This is mathematically more efficient 
 *             than forward-scanning the entire file only to find the end point.
 *          2. **Window Start Calculation**: Because we are scanning backwards, we 
 *             calculate the `startSample` by subtracting the chunk size from the 
 *             current position, ensuring we never read before index 0.
 *          3. **Internal Reverse Loop**: Inside each chunk, we iterate from 
 *             `numThisTime - 1` down to 0. The first sample that trips the threshold 
 *             is guaranteed to be the *absolute last* non-silent sample in the file.
 */
juce::int64 SilenceAnalysisAlgorithms::findSilenceOut(juce::AudioFormatReader &reader,
                                                      float threshold, juce::Thread *thread) {
    const juce::int64 lengthInSamples = reader.lengthInSamples;

    if (reader.numChannels <= 0 || reader.numChannels > kMaxChannels)
        return -1;

    juce::AudioBuffer<float> buffer(reader.numChannels, kChunkSize);

    juce::int64 currentPos = lengthInSamples;
    while (currentPos > 0) {
        // Determine the chunk size, ensuring we don't go past the start of the file
        const int numThisTime = (int)std::min((juce::int64)kChunkSize, currentPos);
        const juce::int64 startSample = currentPos - numThisTime;

        buffer.clear();
        if (!reader.read(&buffer, 0, numThisTime, startSample, true, true))
            return -1;

        if (thread != nullptr) {
            if (thread->threadShouldExit())
                return -1;
            thread->wait(1);
        }

        // Scan backwards through the buffer to find the tail-end of the audio signal
        for (int sample = numThisTime - 1; sample >= 0; --sample) {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                if (std::abs(buffer.getSample(channel, sample)) > threshold) {
                    return startSample + sample;
                }
            }
        }
        // Move the reverse-scanning cursor back by one full chunk
        currentPos -= numThisTime;
    }
    return -1;
}
