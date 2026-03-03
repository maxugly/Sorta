#pragma once
#include <JuceHeader.h>
#include <array>

/**
 * @file DirectoryState.h
 * @ingroup State
 * @brief Data model holding the 10 destination routing paths and current sort mode.
 */
class DirectoryState {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void destinationChanged(int index, const juce::String& path) { juce::ignoreUnused(index, path); }
        virtual void routingModeChanged(bool isCopyMode) { juce::ignoreUnused(isCopyMode); }
    };

    void addListener(Listener* l);
    void removeListener(Listener* l);

    void setDestination(int index, const juce::String& path);
    juce::String getDestination(int index) const;

    void setCopyMode(bool shouldCopy);
    bool getCopyMode() const;

private:
    std::array<juce::String, 10> destinations;
    bool isCopyMode{false}; // false = Move, true = Copy
    juce::ListenerList<Listener> listeners;
    mutable juce::CriticalSection stateLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DirectoryState)
};
