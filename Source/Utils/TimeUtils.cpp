

#include "Utils/TimeUtils.h"

juce::String TimeUtils::formatTime(double seconds, double sampleRate) {
    if (seconds < 0)
        seconds = 0;

    if (sampleRate <= 0)
        sampleRate = 44100.0;

    auto totalSamples = (long long)(seconds * sampleRate + 0.0001);
    auto samplesPerSecond = (long long)sampleRate;

    int hours = (int)(totalSamples / (samplesPerSecond * 3600));
    totalSamples %= (samplesPerSecond * 3600);

    int minutes = (int)(totalSamples / (samplesPerSecond * 60));
    totalSamples %= (samplesPerSecond * 60);

    int secs = (int)(totalSamples / samplesPerSecond);
    int samples = (int)(totalSamples % samplesPerSecond);

    return juce::String::formatted("%02d:%02d:%02d:%05d", hours, minutes, secs, samples);
}

double TimeUtils::parseTime(const juce::String &timeString, double sampleRate) {
    if (sampleRate <= 0)
        sampleRate = 44100.0;

    juce::String cleanTime = timeString.trimCharactersAtStart("+-");

    auto parts = juce::StringArray::fromTokens(cleanTime, ":", "");
    if (parts.size() != 4)
        return -1.0;

    return parts[0].getIntValue() * 3600.0 + parts[1].getIntValue() * 60.0 +
           parts[2].getIntValue() + (double)parts[3].getLargeIntValue() / sampleRate;
}
