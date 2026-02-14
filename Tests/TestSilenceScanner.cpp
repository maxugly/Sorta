#include "../Source/SilenceScanner.h"
#include <iostream>
#include <map>
#include <cassert>

// Mock AudioFormatReader using StubJuce or real JUCE
class MockAudioFormatReader : public juce::AudioFormatReader
{
public:
    MockAudioFormatReader(juce::int64 length, int channels, double rate)
        : AudioFormatReader(nullptr, "MockReader")
    {
        lengthInSamples = length;
        numChannels = channels;
        sampleRate = rate;
        bitsPerSample = 32;
        usesFloatingPointData = true;
    }

    void setSample(int channel, juce::int64 index, float value)
    {
        data[channel][index] = value;
    }

#ifdef TESTING_WITHOUT_JUCE
    // Stub implementation override
    bool read(juce::AudioBuffer<float>* buffer, int startSampleDestBuffer, int numSamplesDestBuffer, juce::int64 readerStartSample, bool useReaderLeftChan, bool useReaderRightChan) override
    {
        for (int i = 0; i < numSamplesDestBuffer; ++i)
        {
            juce::int64 currentPos = readerStartSample + i;
            for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
            {
                float val = 0.0f;
                if (data.count(ch) && data[ch].count(currentPos))
                {
                    val = data[ch][currentPos];
                }
                // Write to buffer
                // Buffer expects writing to its internal storage.
                // Since buffer is StubJuce::AudioBuffer, we can use setSample.
                buffer->setSample(ch, startSampleDestBuffer + i, val);
            }
        }
        return true;
    }
#else
    // Real JUCE implementation override
    bool readSamples(int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                     juce::int64 startSampleInFile, int numSamples) override
    {
        for (int ch = 0; ch < numDestChannels; ++ch)
        {
            if (destSamples[ch] != nullptr)
            {
                float* buffer = reinterpret_cast<float*>(destSamples[ch]) + startOffsetInDestBuffer;
                for (int i = 0; i < numSamples; ++i)
                {
                    juce::int64 currentPos = startSampleInFile + i;
                    float val = 0.0f;
                    if (data.count(ch) && data[ch].count(currentPos))
                    {
                        val = data[ch][currentPos];
                    }
                    buffer[i] = val;
                }
            }
        }
        return true;
    }
#endif

private:
    std::map<int, std::map<juce::int64, float>> data;
};

void test_full_silence()
{
    MockAudioFormatReader reader(100000, 2, 44100.0);
    juce::int64 start = SilenceScanner::findFirstSoundSample(&reader, 0.01f);
    assert(start == -1);
    juce::int64 end = SilenceScanner::findLastSoundSample(&reader, 0.01f);
    assert(end == -1);
    std::cout << "test_full_silence passed" << std::endl;
}

void test_sound_at_start()
{
    MockAudioFormatReader reader(100000, 2, 44100.0);
    reader.setSample(0, 0, 0.5f);
    juce::int64 start = SilenceScanner::findFirstSoundSample(&reader, 0.01f);
    assert(start == 0);
    std::cout << "test_sound_at_start passed" << std::endl;
}

void test_sound_at_end()
{
    MockAudioFormatReader reader(100000, 2, 44100.0);
    reader.setSample(1, 99999, 0.5f); // Last sample
    juce::int64 end = SilenceScanner::findLastSoundSample(&reader, 0.01f);
    assert(end == 99999);
    std::cout << "test_sound_at_end passed" << std::endl;
}

void test_middle_sound()
{
    MockAudioFormatReader reader(100000, 2, 44100.0);
    reader.setSample(0, 50000, 0.5f);
    juce::int64 start = SilenceScanner::findFirstSoundSample(&reader, 0.01f);
    assert(start == 50000);
    juce::int64 end = SilenceScanner::findLastSoundSample(&reader, 0.01f);
    assert(end == 50000);
    std::cout << "test_middle_sound passed" << std::endl;
}

void test_chunk_boundary()
{
    // Chunk size is 65536.
    MockAudioFormatReader reader(200000, 2, 44100.0);

    // Test boundary at 65536
    reader.setSample(0, 65536, 0.5f);
    juce::int64 start = SilenceScanner::findFirstSoundSample(&reader, 0.01f);
    assert(start == 65536);

    MockAudioFormatReader reader2(200000, 2, 44100.0);
    reader2.setSample(0, 65535, 0.5f);
    start = SilenceScanner::findFirstSoundSample(&reader2, 0.01f);
    assert(start == 65535);

    std::cout << "test_chunk_boundary passed" << std::endl;
}

void test_large_file_simulation()
{
    // Simulate a large file index without large memory
    juce::int64 largeSize = 300000; // Larger than a few chunks
    MockAudioFormatReader reader(largeSize, 2, 44100.0);
    reader.setSample(0, largeSize - 1, 0.5f);

    juce::int64 end = SilenceScanner::findLastSoundSample(&reader, 0.01f);
    assert(end == largeSize - 1);

    std::cout << "test_large_file_simulation passed" << std::endl;
}

int main()
{
    test_full_silence();
    test_sound_at_start();
    test_sound_at_end();
    test_middle_sound();
    test_chunk_boundary();
    test_large_file_simulation();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
