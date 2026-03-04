#include "UI/Views/BottomPanelView.h"
#include "Utils/Config.h"

BottomPanelView::BottomPanelView() {
    addAndMakeVisible(placeholderEditor);
    placeholderEditor.applyStandardStyle(juce::Justification::centred);
    placeholderEditor.setText("Bottom Panel Placeholder", juce::dontSendNotification);
    placeholderEditor.setReadOnly(true);
}

void BottomPanelView::resized() {
    placeholderEditor.setBounds(getLocalBounds().reduced(Config::Layout::windowBorderMargins, Config::Layout::BottomBar::verticalMargin));
}
