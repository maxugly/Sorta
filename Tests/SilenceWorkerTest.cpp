#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <algorithm>
#include <vector>
#include "SilenceAnalysisWorker.h"

// Mock AudioFormatReader for testing
class MockAudioFormatReader : public juce::AudioFormatReader
{
public:
    MockAudioFormatReader(int numChannels, juce::int64 lengthInSamples, double sampleRate, juce::int64 silenceEndSample)
        : juce::AudioFormatReader(nullptr, "MockReader"),
          silenceEndSample(silenceEndSample)
    {
        this->numChannels = (unsigned int)numChannels;
        this->sampleRate = sampleRate;
        this->bitsPerSample = 32;
        this->lengthInSamples = lengthInSamples;
        usesFloatingPointData = true;
    }

    bool readSamples(int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
                     juce::int64 startSampleInFile, int numSamples) override
    {
        for (int ch = 0; ch < numDestChannels; ++ch)
        {
            if (destSamples[ch] == nullptr) continue;

            float* buffer = reinterpret_cast<float*>(destSamples[ch]) + startOffsetInDestBuffer;
            for (int i = 0; i < numSamples; ++i)
            {
                juce::int64 currentSample = startSampleInFile + i;
                if (currentSample < silenceEndSample)
                    buffer[i] = 0.0f; // Silence
                else
                    buffer[i] = 1.0f; // Signal
            }
        }
        return true;
    }

private:
    juce::int64 silenceEndSample;
};

class SilenceWorkerTest : public juce::UnitTest
{
public:
    SilenceWorkerTest() : juce::UnitTest("Silence Analysis Benchmark") {}

    void runTest() override
    {
        beginTest("Async Chunked Implementation");

        const int numChannels = 2;
        const double sampleRate = 44100.0;
        const juce::int64 totalLength = 10 * 44100; // 10 seconds
        const juce::int64 silenceLength = 5 * 44100; // 5 seconds of silence

        MockAudioFormatReader reader(numChannels, totalLength, sampleRate, silenceLength);

        SilenceAnalysisWorker worker;

        juce::int64 foundSample = -1;
        bool finished = false;

        worker.onSilenceFound = [&](juce::int64 sample, double rate) {
            foundSample = sample;
            finished = true;
        };

        worker.onNoSilenceFound = [&](juce::String) {
            finished = true;
        };

        worker.onError = [&](juce::String msg) {
            logMessage("Error: " + msg);
            finished = true;
        };

        // Start detection
        worker.startInDetection(&reader, 0.001f);

        // Measure execution time of chunks
        double maxChunkTime = 0.0;
        int chunkCount = 0;

        auto startTime = juce::Time::getMillisecondCounterHiRes();

        while (worker.isWorking())
        {
            auto chunkStart = juce::Time::getMillisecondCounterHiRes();

            worker.processNextChunk();

            auto chunkEnd = juce::Time::getMillisecondCounterHiRes();
            double duration = chunkEnd - chunkStart;
            if (duration > maxChunkTime) maxChunkTime = duration;
            chunkCount++;

            // Safety break
            if (chunkCount > 10000)
            {
                expect(false, "Infinite loop detected");
                break;
            }
        }

        auto endTime = juce::Time::getMillisecondCounterHiRes();

        expect(finished);
        expectEquals(foundSample, silenceLength);

        logMessage("Total time: " + juce::String(endTime - startTime) + " ms");
        logMessage("Max Chunk Time: " + juce::String(maxChunkTime) + " ms");
        logMessage("Chunk Count: " + juce::String(chunkCount));

        // Assert that max chunk time is small (e.g. < 5ms)
        // Baseline blocking was ~6ms.
        // Chunk time should be much smaller because chunks are 65536 samples?
        // Wait, baseline was doing ALL chunks in one go.
        // So baseline time was Sum(ChunkTimes).
        // Max Chunk Time here is one chunk.
        // 6ms / (10s / 1.5s/chunk) = 6ms / 7 chunks = ~1ms.
        // So expected max chunk time is ~1ms.
        // Allowing some overhead for test environment
        expectLessThan(maxChunkTime, 5.0);
    }
};

static SilenceWorkerTest silenceWorkerTest;
