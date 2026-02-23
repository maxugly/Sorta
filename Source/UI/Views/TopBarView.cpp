/**
 * @file TopBarView.cpp
 */

#include "UI/Views/TopBarView.h"
#include "UI/ControlPanel.h"
#include "UI/Components/TransportStrip.h"
#include "UI/Views/VolumeView.h"
#include "Utils/Config.h"

TopBarView::TopBarView(ControlPanel &cp) : controlPanel(cp) {
    addAndMakeVisible(openButton);
    addAndMakeVisible(modeButton);
    addAndMakeVisible(exitButton);
    addAndMakeVisible(statsButton);
    addAndMakeVisible(channelViewButton);
    addAndMakeVisible(eyeCandyButton);

    transportStrip = std::make_unique<TransportStrip>();
    addAndMakeVisible(transportStrip.get());

    volumeView = std::make_unique<VolumeView>();
    addAndMakeVisible(volumeView.get());
}

TopBarView::~TopBarView() = default;

void TopBarView::resized() {
    auto topRow = getLocalBounds();
    const int margin = Config::Layout::windowBorderMargins;
    const int buttonWidth = Config::Layout::buttonWidth;
    const int spacing = (int)Config::UI::GroupSpacing;

    // Open button (Alone)
    openButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);

    // Transport Group (Modular TransportStrip)
    if (transportStrip != nullptr) {
        const int stripWidth = (buttonWidth * 5) + (spacing * 4);
        transportStrip->setBounds(topRow.removeFromLeft(stripWidth));
    }
    topRow.removeFromLeft(margin);

    if (volumeView != nullptr) {
        const int knobSize = Config::Layout::TopBar::volumeKnobSize;
        volumeView->setBounds(topRow.removeFromLeft(knobSize));
    }
    topRow.removeFromLeft(margin);

    exitButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(margin);

    // View Group (Left, Middle, Right) - Grouped and right-aligned
    channelViewButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    statsButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    modeButton.setBounds(topRow.removeFromRight(buttonWidth));

    // Eye Candy Toggle (Alone) - Left of View Group
    topRow.removeFromRight(margin);
    eyeCandyButton.setBounds(topRow.removeFromRight((int)Config::UI::WidgetUnit));
}
