#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace juce {
    using int64 = long long;

    template <typename T>
    class AudioBuffer {
    public:
        AudioBuffer(int channels, int size) : numCh(channels), numSamp(size) {
            data.resize(channels * size);
        }
        void clear() {
            std::fill(data.begin(), data.end(), 0);
        }
        T getSample(int channel, int index) const {
            if (channel < 0 || channel >= numCh || index < 0 || index >= numSamp) return 0;
            return data[channel * numSamp + index];
        }
        void setSample(int channel, int index, T value) {
             if (channel < 0 || channel >= numCh || index < 0 || index >= numSamp) return;
            data[channel * numSamp + index] = value;
        }

        int getNumChannels() const { return numCh; }
        int getNumSamples() const { return numSamp; }

        std::vector<T> data;
        int numCh;
        int numSamp;
    };

    class AudioFormatReader {
    public:
        AudioFormatReader(void*, const char*) {}
        virtual ~AudioFormatReader() {}

        int64 lengthInSamples = 0;
        int numChannels = 0;
        double sampleRate = 0;
        int bitsPerSample = 0;
        bool usesFloatingPointData = false;

        virtual bool read(AudioBuffer<float>* buffer, int startSampleDestBuffer, int numSamplesDestBuffer, int64 readerStartSample, bool useReaderLeftChan, bool useReaderRightChan) {
            return false; // Stub default
        }

        // Helper to avoid build errors if code calls other methods
        virtual bool readSamples(int** destSamples, int numDestChannels, int startOffsetInDestBuffer, int64 startSampleInFile, int numSamples) {
            return false;
        }
    };

    template <typename T>
    T jmin(T a, T b) { return std::min(a, b); }
}
