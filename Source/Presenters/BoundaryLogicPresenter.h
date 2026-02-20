#ifndef AUDIOFILER_BOUNDARYLOGICPRESENTER_H
#define AUDIOFILER_BOUNDARYLOGICPRESENTER_H

#if defined(JUCE_HEADLESS)
    #include <juce_gui_basics/juce_gui_basics.h>
#else
    #include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"

class ControlPanel;
class SilenceDetector;

/**
 * @class BoundaryLogicPresenter
 * @brief Manages the manual text entry and logic for Cut In/Out boundaries.
 */
class BoundaryLogicPresenter : private juce::TextEditor::Listener,
                               public juce::MouseListener,
                               public PlaybackTimerManager::Listener
{
public:
    BoundaryLogicPresenter(ControlPanel& ownerPanel,
                           SilenceDetector& detector,
                           juce::TextEditor& cutIn,
                           juce::TextEditor& cutOut);
    ~BoundaryLogicPresenter() override;

    void initialiseEditors();
    void refreshLabels();

    void setCutStartFromSample(int sampleIndex);
    void setCutEndFromSample(int sampleIndex);

    void ensureCutOrder();

    // PlaybackTimerManager::Listener
    void playbackTimerTick() override { refreshLabels(); }
    void animationUpdate(float breathingPulse) override { juce::ignoreUnused(breathingPulse); }

private:
    // TextEditor::Listener
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;

    // MouseListener
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void syncEditorToPosition(juce::TextEditor& editor, double positionSeconds);
    bool applyCutInFromEditor(double newPosition, juce::TextEditor& editor);
    bool applyCutOutFromEditor(double newPosition, juce::TextEditor& editor);
    
    void setCutInPosition(double positionSeconds);
    void setCutOutPosition(double positionSeconds);
    
    double getAudioTotalLength() const;

    ControlPanel& owner;
    SilenceDetector& silenceDetector;
    juce::TextEditor& cutInEditor;
    juce::TextEditor& cutOutEditor;
    
    bool isEditingIn{false};
    bool isEditingOut{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoundaryLogicPresenter)
};

#endif
