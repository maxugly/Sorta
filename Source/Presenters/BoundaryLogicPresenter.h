#ifndef AUDIOFILER_BOUNDARYLOGICPRESENTER_H
#define AUDIOFILER_BOUNDARYLOGICPRESENTER_H

#if defined(JUCE_HEADLESS)
#include <juce_gui_basics/juce_gui_basics.h>
#else
#include <JuceHeader.h>
#endif

#include "Presenters/PlaybackTimerManager.h"
#include "Core/SessionState.h"

class ControlPanel;

/**
 * @class BoundaryLogicPresenter

 * @brief Manages the manual text entry and logic for Cut In/Out boundaries.
 */
class BoundaryLogicPresenter final : public juce::TextEditor::Listener,
                                 public juce::MouseListener,
                                 public SessionState::Listener {
  public:
    BoundaryLogicPresenter(ControlPanel &ownerPanel, 
                           juce::TextEditor &cutIn, juce::TextEditor &cutOut);
    ~BoundaryLogicPresenter() override;


    void initialiseEditors();
    void refreshLabels();

    void ensureCutOrder();

    // SessionState::Listener
    void fileChanged(const juce::String &filePath) override {
        juce::ignoreUnused(filePath);
        refreshLabels();
    }
    void cutInChanged(double) override { 
        ensureCutOrder(); 
        refreshLabels();
    }
    void cutOutChanged(double) override { 
        ensureCutOrder(); 
        refreshLabels();
    }
    void cutPreferenceChanged(const MainDomain::CutPreferences&) override {
        refreshLabels();
    }

  private:
    // TextEditor::Listener
    void textEditorTextChanged(juce::TextEditor &editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor &editor) override;
    void textEditorFocusLost(juce::TextEditor &editor) override;

    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    void syncEditorToPosition(juce::TextEditor &editor, double positionSeconds);
    bool applyCutFromEditor(juce::TextEditor &editor, double newPosition);
    void setCutPosition(bool isIn, double positionSeconds);

    double getAudioTotalLength() const;
private:
  ControlPanel &owner;
  juce::TextEditor &cutInEditor;
  juce::TextEditor &cutOutEditor;


    bool isEditingIn{false};
    bool isEditingOut{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoundaryLogicPresenter)
};

#endif
