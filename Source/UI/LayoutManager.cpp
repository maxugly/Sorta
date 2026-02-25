

#include "UI/LayoutManager.h"

#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

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
               Config::UI::ResetButtonWidthUnits * 2 + Config::UI::ThresholdWidthUnits) *
              unit) + (spacing * 5);

    if (controlPanel.inStrip != nullptr)
        controlPanel.inStrip->setBounds(cutRow.removeFromLeft(stripWidth));

    if (controlPanel.outStrip != nullptr)
        controlPanel.outStrip->setBounds(cutRow.removeFromRight(stripWidth));

    // --- NEW: Center the Cut Length Strip in the remaining space ---
    if (controlPanel.getCutLengthStrip() != nullptr) {
        const int lengthWidth = 220 + (int)Config::UI::WidgetUnit;
        controlPanel.getCutLengthStrip()->setBounds(cutRow.withSizeKeepingCentre(lengthWidth, height));
    }
}

void LayoutManager::layoutBottomRowAndTextDisplay(juce::Rectangle<int> &bounds, int rowHeight) {
    const int margin = Config::Layout::windowBorderMargins;

    auto bottomRow = bounds.removeFromBottom(rowHeight).reduced(margin);
    
    controlPanel.layoutCache.contentAreaBounds = bounds.reduced(margin);

    const auto fullBounds = controlPanel.getLocalBounds();

    if (controlPanel.playbackTimeView != nullptr) {
        const int textY = bottomRow.getY() - Config::Layout::Text::playbackOffsetY;
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
