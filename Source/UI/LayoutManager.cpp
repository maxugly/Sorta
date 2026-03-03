

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

    // 1. Structural Horizontal Split: All controls/waveform (left) vs Sidebar (right)
    // Note: comps array is: dummy (left-container-math), resizer, directoryRoutingView
    juce::Component* hComps[] = { &controlPanel.leftWorkspaceAnchor, controlPanel.horizontalResizer.get(), &controlPanel.directoryRoutingView };
    controlPanel.horizontalLayoutManager.layOutComponents(hComps, 3, fullBounds.getX(), fullBounds.getY(), fullBounds.getWidth(), fullBounds.getHeight(), false, true);

    // 2. Adjust right side for Exit Button
    const int buttonWidth = Config::Layout::buttonWidth;
    controlPanel.exitButton.setBounds(fullBounds.getRight() - buttonWidth - margin, margin, 
                                      buttonWidth, (int)Config::UI::WidgetHeight);
    
    auto routingBounds = controlPanel.directoryRoutingView.getBounds();
    routingBounds.setTop(controlPanel.exitButton.getBottom() + margin);
    controlPanel.directoryRoutingView.setBounds(routingBounds);

    // 3. Vertical layout inside the left workspace anchor
    auto bounds = controlPanel.leftWorkspaceAnchor.getBounds();
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
    
    // Always use the structural center cut-out as the content anchor
    controlPanel.layoutCache.contentAreaBounds = bounds.reduced(margin);

    controlPanel.getPresenterCore().getStatsPresenter().layoutWithin(
        controlPanel.layoutCache.contentAreaBounds);
}

void LayoutManager::layoutWaveformArea() {
    auto* wcv = controlPanel.getWaveformCanvasView();
    if (wcv == nullptr) return;

    const int margin = Config::Layout::windowBorderMargins;
    auto contentArea = controlPanel.layoutCache.contentAreaBounds;
    auto fullLeftArea = controlPanel.leftWorkspaceAnchor.getBounds();

    // 1. Math Phase: Execute Vertical Layout on the invisible ANCHOR component
    // We use the FULL width of the left area so the resizer bar shoots edge-to-edge!
    juce::Component* vComps[] = { &controlPanel.waveformLayoutAnchor, controlPanel.verticalResizer.get(), &controlPanel.fileQueuePlaceholder };
    controlPanel.verticalLayoutManager.layOutComponents(vComps, 3,
        fullLeftArea.getX(), 
        contentArea.getY(), 
        fullLeftArea.getWidth(), 
        contentArea.getHeight(), 
        true, true);

    // 2. Extract the uncorrupted structural bounds
    auto waveAnchorBounds = controlPanel.waveformLayoutAnchor.getBounds();

    // 3. Visual Phase: Apply horizontal margins for the visual components
    auto waveVisualBounds = waveAnchorBounds.reduced(margin, 0);
    
    // Also apply horizontal margins to the file queue so it perfectly aligns with the waveform
    controlPanel.fileQueuePlaceholder.setBounds(controlPanel.fileQueuePlaceholder.getBounds().reduced(margin, 0));

    // 4. Anchor playback timers securely to the visual bounds BEFORE we stretch the top
    if (auto* ptv = controlPanel.getPlaybackTimeView()) {
        const int textHeight = Config::Layout::Text::playbackHeight;
        ptv->setBounds(waveVisualBounds.getX(), 
                       waveVisualBounds.getBottom() - textHeight - margin, 
                       waveVisualBounds.getWidth(), 
                       textHeight);
    }

    // 5. Adjust vertical stretching for Overlay Mode
    if (controlPanel.getSessionState().getViewMode() == AppEnums::ViewMode::Overlay) {
        waveVisualBounds.setTop(0); // Stretch flush to the absolute ceiling
    }

    // 6. Push the mathematically perfect bounds to the Canvas View
    wcv->setBounds(waveVisualBounds);
    controlPanel.layoutCache.waveformBounds = waveVisualBounds;
}
