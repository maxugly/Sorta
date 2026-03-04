/**
 * @file TopBarView.cpp
 */

#include "UI/Views/TopBarView.h"
#include "UI/ControlPanel.h"
#include "UI/Components/TransportStrip.h"
#include "UI/Views/VolumeView.h"
#include "Utils/Config.h"

TopBarView::TopBarView(ControlPanel &cp) : controlPanel(cp) {
    addAndMakeVisible(modeButton);
    addAndMakeVisible(statsButton);
    addAndMakeVisible(channelViewButton);

    transportStrip = std::make_unique<TransportStrip>();
    addAndMakeVisible(transportStrip.get());

    volumeView = std::make_unique<VolumeView>();
    addAndMakeVisible(volumeView.get());
    volumeView->getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);

    matrixView = std::make_unique<MatrixView>();
    addAndMakeVisible(matrixView.get());
    matrixView->getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);

    hintView = std::make_unique<HintView>();
    addAndMakeVisible(hintView.get());
    hintView->getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);

    addAndMakeVisible(themeSelector);
    themeSelector.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);
    
    addAndMakeVisible(themeUpButton);
    themeUpButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    
    addAndMakeVisible(themeDownButton);
    themeDownButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Middle);
    
    themeUpButton.setButtonText(Config::Labels::themeUp);
    themeDownButton.setButtonText(Config::Labels::themeDown);
}

TopBarView::~TopBarView() = default;

void TopBarView::resized() {
    auto topRow = getLocalBounds();
    const int margin = Config::Layout::windowBorderMargins;
    const int buttonWidth = Config::Layout::buttonWidth;
    const int spacing = (int)Config::UI::GroupSpacing;

    // 1. Left Strip: [Transport (Play | Stop | etc)] + [Volume]
    const int knobSize = Config::Layout::TopBar::volumeKnobSize;
    if (transportStrip != nullptr) {
        const int stripWidth = (buttonWidth * 5) + (spacing * 5) + knobSize;
        auto leftGroup = topRow.removeFromLeft(stripWidth);
        
        transportStrip->setBounds(leftGroup.removeFromLeft(leftGroup.getWidth() - knobSize));
        if (volumeView) volumeView->setBounds(leftGroup);
    }

    // 2. Right Strip: [Theme Controls | Mode | Stats | Channels]
    channelViewButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    statsButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    modeButton.setBounds(topRow.removeFromRight(buttonWidth));
    topRow.removeFromRight(spacing);
    
    themeDownButton.setBounds(topRow.removeFromRight(buttonWidth / 2));
    topRow.removeFromRight(0); // No gap between up/down
    themeUpButton.setBounds(topRow.removeFromRight(buttonWidth / 2));
    topRow.removeFromRight(0); // No gap between selector/up
    themeSelector.setBounds(topRow.removeFromRight(Config::Layout::TopBar::themeSelectorWidth));

    // 3. Center Strip: [Matrix | Hint]
    topRow.reduce(margin, 0); // Structural padding before the center strip
    const int matrixWidth = (Config::Layout::Matrix::columns * Config::Layout::Matrix::squareSize) + Config::Layout::Matrix::padding;
    
    if (matrixView) matrixView->setBounds(topRow.removeFromLeft(matrixWidth));
    if (hintView) hintView->setBounds(topRow);
}
