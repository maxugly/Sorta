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
    themeSelector.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    
    addAndMakeVisible(themeUpButton);
    themeUpButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    
    addAndMakeVisible(themeDownButton);
    themeDownButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
    
    themeUpButton.setButtonText(Config::Labels::themeUp);
    themeDownButton.setButtonText(Config::Labels::themeDown);
}

TopBarView::~TopBarView() = default;

void TopBarView::resized() {
    auto topRow = getLocalBounds();
    const int margin = Config::Layout::windowBorderMargins;
    const int buttonWidth = Config::Layout::buttonWidth;
    const int spacing = (int)Config::UI::GroupSpacing;

    // 1. Left Anchored: Open and Transport
    openButton.setBounds(topRow.removeFromLeft(buttonWidth));
    topRow.removeFromLeft(margin);

    if (transportStrip != nullptr) {
        const int stripWidth = (buttonWidth * Config::Layout::TopBar::transportButtonsCount) + 
                               (spacing * Config::Layout::TopBar::transportButtonsSpacingCount);
        transportStrip->setBounds(topRow.removeFromLeft(stripWidth));
    }

    // 2. Right Anchored: View Mode controls
    channelViewButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    statsButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    modeButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(margin);
    
    themeDownButton.setBounds(topRow.removeFromRight(buttonWidth / 2));
    themeUpButton.setBounds(topRow.removeFromRight(buttonWidth / 2));
    themeSelector.setBounds(topRow.removeFromRight(Config::Layout::TopBar::themeSelectorWidth));

    // 3. Centered Strip: [Volume | Matrix | Hint]
    const int knobSize = Config::Layout::TopBar::volumeKnobSize;
    const int matrixWidth = (Config::Layout::Matrix::columns * Config::Layout::Matrix::squareSize) + Config::Layout::Matrix::padding;
    const int hintWidth = Config::Layout::TopBar::hintWidth;
    const int totalCenterWidth = knobSize + spacing + matrixWidth + spacing + hintWidth;

    auto centerArea = getLocalBounds().withSizeKeepingCentre(totalCenterWidth, getHeight());
    if (volumeView) volumeView->setBounds(centerArea.removeFromLeft(knobSize));
    centerArea.removeFromLeft(spacing);
    if (matrixView) matrixView->setBounds(centerArea.removeFromLeft(matrixWidth));
    centerArea.removeFromLeft(spacing);
    if (hintView) hintView->setBounds(centerArea.removeFromLeft(hintWidth));
}
