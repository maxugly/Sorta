#include "WaveformManager.h"

WaveformManager::WaveformManager(juce::AudioFormatManager& formatManagerIn)
    : formatManager(formatManagerIn)
{
}

void WaveformManager::loadFile(const juce::File& file)
{
    thumbnail.setSource(new juce::FileInputSource(file));
}

juce::AudioThumbnail& WaveformManager::getThumbnail()
{
    return thumbnail;
}

void WaveformManager::addChangeListener(juce::ChangeListener* listener)
{
    thumbnail.addChangeListener(listener);
}

void WaveformManager::removeChangeListener(juce::ChangeListener* listener)
{
    thumbnail.removeChangeListener(listener);
}
