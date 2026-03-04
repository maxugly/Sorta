#include "UI/Views/BottomPanelView.h"
#include "Utils/Config.h"

BottomPanelView::BottomPanelView() {
    addAndMakeVisible(openButton);
    addAndMakeVisible(placeholderEditor);
    placeholderEditor.applyStandardStyle(juce::Justification::centred);
    placeholderEditor.setText("Bottom Panel Placeholder", juce::dontSendNotification);
    placeholderEditor.setReadOnly(true);
}

void BottomPanelView::resized() {
    auto b = getLocalBounds().reduced(Config::Layout::windowBorderMargins, Config::Layout::BottomBar::verticalMargin);
    
    openButton.setBounds(b.removeFromLeft(Config::Layout::buttonWidth));
    b.removeFromLeft(Config::Layout::windowBorderMargins); // Margin after button
    
    placeholderEditor.setBounds(b);
}
