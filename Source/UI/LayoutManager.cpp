

#include "UI/LayoutManager.h"

#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/PlaybackTimeView.h"
#include "Utils/Config.h"
#include "Utils/TimeUtils.h"

LayoutManager::LayoutManager(ControlPanel &controlPanelIn) : controlPanel(controlPanelIn) {
}

void LayoutManager::performLayout() {
    auto fullBounds = controlPanel.getLocalBounds();
    const int margin = Config::Layout::windowBorderMargins;

    // 1. Execute the horizontal split on the ENTIRE window first.
    // This defines the boundary between all controls/waveform (left) and DirectoryRouting/Exit (right).
    // Note: comps array is: dummy (left-container-math), resizer, directoryRoutingView
    juce::Component* hComps[] = { nullptr, controlPanel.horizontalResizer.get(), &controlPanel.directoryRoutingView };
    controlPanel.horizontalLayoutManager.layOutComponents(hComps, 3, fullBounds.getX(), fullBounds.getY(), fullBounds.getWidth(), fullBounds.getHeight(), false, true);

    // 2. Adjust right side for Exit Button.
    // Fixed buttonWidth ensures it does not stretch.
    const int buttonWidth = Config::Layout::buttonWidth;
    controlPanel.exitButton.setBounds(fullBounds.getRight() - buttonWidth - margin, margin, 
                                      buttonWidth, (int)Config::UI::WidgetHeight);
    
    // DirectoryRoutingView fills the rest of the right column below the exit button
    auto routingBounds = controlPanel.directoryRoutingView.getBounds();
    routingBounds.setTop(controlPanel.exitButton.getBottom() + margin);
    controlPanel.directoryRoutingView.setBounds(routingBounds);

    // Everything on the left is constrained by the main container's new bounds.
    auto leftBounds = fullBounds.withWidth(controlPanel.horizontalResizer->getX());

    // 3. Perform standard vertical layout inside the leftBounds.
    auto bounds = leftBounds;
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
        const int lengthWidth = Config::Layout::cutLengthStripWidth;
        controlPanel.getCutLengthStrip()->setBounds(cutRow.withSizeKeepingCentre(lengthWidth, height));
    }
}

void LayoutManager::layoutBottomRowAndTextDisplay(juce::Rectangle<int> &bounds, int rowHeight) {
    juce::ignoreUnused(rowHeight);
    const int margin = Config::Layout::windowBorderMargins;

    // Do not chop off the bottom row anymore; let the waveform use this space!
    controlPanel.layoutCache.contentAreaBounds = bounds.reduced(margin);
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

void LayoutManager::layoutWaveformArea() {
    auto* wcv = controlPanel.getWaveformCanvasView();
    if (wcv == nullptr) return;

    // Inside the already-constrained waveformBounds, only do the vertical split
    auto wb = controlPanel.layoutCache.waveformBounds;
    juce::Component* vComps[] = { wcv, controlPanel.verticalResizer.get(), &controlPanel.fileQueuePlaceholder };
    controlPanel.verticalLayoutManager.layOutComponents(vComps, 3, wb.getX(), wb.getY(), wb.getWidth(), wb.getHeight(), true, true);
    
    if (auto* ptv = controlPanel.getPlaybackTimeView()) {
        auto viewBounds = wcv->getBounds();
        const int textHeight = Config::Layout::Text::playbackHeight;
        const int margin = Config::Layout::windowBorderMargins;
        ptv->setBounds(viewBounds.getX(), viewBounds.getBottom() - textHeight - margin, viewBounds.getWidth(), textHeight);
    }
}
