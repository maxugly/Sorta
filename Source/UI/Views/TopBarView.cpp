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

    transportStrip = std::make_unique<TransportStrip>();
    addAndMakeVisible(transportStrip.get());

    volumeView = std::make_unique<VolumeView>();
    addAndMakeVisible(volumeView.get());

    matrixView = std::make_unique<MatrixView>();
    addAndMakeVisible(matrixView.get());

    hintView = std::make_unique<HintView>();
    addAndMakeVisible(hintView.get());

    addAndMakeVisible(themeSelector);
    themeSelector.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);

    addAndMakeVisible(themeUpButton);
    addAndMakeVisible(themeDownButton);
    themeUpButton.setButtonText(Config::Labels::themeUp);
    themeDownButton.setButtonText(Config::Labels::themeDown);
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
        const int stripWidth = (buttonWidth * Config::Layout::TopBar::transportButtonsCount) + 
                               (spacing * Config::Layout::TopBar::transportButtonsSpacingCount);
        transportStrip->setBounds(topRow.removeFromLeft(stripWidth));
    }
    topRow.removeFromLeft(margin);

    // Group: [Volume | Matrix | Hint]
    if (volumeView != nullptr) {
        const int knobSize = Config::Layout::TopBar::volumeKnobSize;
        volumeView->setBounds(topRow.removeFromLeft(knobSize));
    }
    
    if (matrixView != nullptr) {
        const int sqSize = Config::Layout::Matrix::squareSize;
        const int cols = Config::Layout::Matrix::columns;
        const int padding = Config::Layout::Matrix::padding;
        const int matrixWidth = (cols * sqSize) + padding;
        matrixView->setBounds(topRow.removeFromLeft(matrixWidth));
    }

    if (hintView != nullptr) {
        const int hintWidth = Config::Layout::TopBar::hintWidth;
        hintView->setBounds(topRow.removeFromLeft(hintWidth));
    }

    themeSelector.setBounds(topRow.removeFromLeft(Config::Layout::TopBar::themeSelectorWidth));
    themeUpButton.setBounds(topRow.removeFromLeft(buttonWidth / 2));
    themeDownButton.setBounds(topRow.removeFromLeft(buttonWidth / 2));

    topRow.removeFromLeft(margin);

    exitButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(margin);

    // View Group (Left, Middle, Right) - Grouped and right-aligned
    channelViewButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    statsButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    modeButton.setBounds(topRow.removeFromRight(buttonWidth));
}
