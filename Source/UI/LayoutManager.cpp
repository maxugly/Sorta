

#include "UI/LayoutManager.h"

#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Workers/SilenceDetector.h"

LayoutManager::LayoutManager(ControlPanel &controlPanelIn) : controlPanel(controlPanelIn) {
}

void LayoutManager::performLayout() {
    auto bounds = controlPanel.getLocalBounds();
    const int margin = Config::Layout::windowBorderMargins;
    const int rowHeight = (int)Config::UI::WidgetHeight + margin * 2;

    layoutTopRowButtons(bounds, rowHeight);

    layoutCutControls(bounds, rowHeight);

    layoutBottomRowAndTextDisplay(bounds, rowHeight);

    layoutWaveformAndStats(bounds);
}

void LayoutManager::layoutTopRowButtons(juce::Rectangle<int> &bounds, int rowHeight) {
    juce::ignoreUnused(rowHeight);
    const int margin = Config::Layout::windowBorderMargins;
    const int height = (int)Config::UI::WidgetHeight;

    auto topRow = bounds.removeFromTop(height + margin * 2).reduced(margin);
    topRow.setHeight(height);

    if (controlPanel.topBarView != nullptr)
        controlPanel.topBarView->setBounds(topRow);
}

void LayoutManager::layoutCutControls(juce::Rectangle<int> &bounds, int rowHeight) {
    juce::ignoreUnused(rowHeight);
    const int margin = Config::Layout::windowBorderMargins;
    const float unit = Config::UI::WidgetUnit;
    const int spacing = (int)Config::UI::GroupSpacing;
    const int height = (int)Config::UI::WidgetHeight;

    auto cutRow = bounds.removeFromTop(height + margin * 2).reduced(margin);
    cutRow.setHeight(height);

    const int stripWidth =
        (int)((Config::UI::CutButtonWidthUnits * 2 + Config::UI::TimerWidthUnits +
               Config::UI::ResetButtonWidthUnits + Config::UI::ThresholdWidthUnits) *
              unit) +
        (spacing * 4);

    if (controlPanel.inStrip != nullptr)
        controlPanel.inStrip->setBounds(cutRow.removeFromLeft(stripWidth));

    if (controlPanel.outStrip != nullptr)
        controlPanel.outStrip->setBounds(cutRow.removeFromRight(stripWidth));
}

void LayoutManager::layoutBottomRowAndTextDisplay(juce::Rectangle<int> &bounds, int rowHeight) {
    const int margin = Config::Layout::windowBorderMargins;
    const int buttonWidth = Config::Layout::buttonWidth;
    const int playbackWidth = Config::Layout::Text::playbackWidth;

    auto bottomRow = bounds.removeFromBottom(rowHeight).reduced(margin);
    
    if (controlPanel.getMatrixView() != nullptr) {
        const int sqSize = Config::Layout::Matrix::squareSize;
        const int rows = Config::Layout::Matrix::rows;
        // 8 LEDs total (based on presenter logic), so 2 columns of 4 rows.
        const int cols = 2; 
        auto matrixBounds = bottomRow.removeFromLeft(cols * sqSize).withHeight(rows * sqSize);
        controlPanel.getMatrixView()->setBounds(matrixBounds.withY(bottomRow.getCentreY() - matrixBounds.getHeight() / 2));
    }

    controlPanel.layoutCache.bottomRowTopY = bottomRow.getY();
    controlPanel.layoutCache.contentAreaBounds = bounds.reduced(margin);

    const auto fullBounds = controlPanel.getLocalBounds();
    controlPanel.layoutCache.playbackLeftTextX = fullBounds.getX() + margin;
    controlPanel.layoutCache.playbackCenterTextX =
        (fullBounds.getWidth() / 2) - (playbackWidth / 2);
    controlPanel.layoutCache.playbackRightTextX = fullBounds.getRight() - margin - playbackWidth;

    if (controlPanel.playbackTimeView != nullptr) {
        const int textY = controlPanel.layoutCache.bottomRowTopY - Config::Layout::Text::playbackOffsetY;
        controlPanel.playbackTimeView->setBounds(0, textY, fullBounds.getWidth(),
                                                Config::Layout::Text::playbackHeight);
    }
}

void LayoutManager::layoutWaveformAndStats(juce::Rectangle<int> &bounds) {
    const int margin = Config::Layout::windowBorderMargins;
    if (controlPanel.getSessionState().getViewMode() == AppEnums::ViewMode::Overlay) {
        controlPanel.layoutCache.waveformBounds = controlPanel.getLocalBounds();
    } else {
        controlPanel.layoutCache.waveformBounds = bounds.reduced(margin);
    }

    controlPanel.getPresenterCore().getStatsPresenter().layoutWithin(
        controlPanel.layoutCache.contentAreaBounds);
}
