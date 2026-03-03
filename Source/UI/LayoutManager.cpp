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
    const int height = (int)Config::UI::WidgetHeight;

    // 1. Horizontal Split: All controls/waveform (left) vs Sidebar (right)
    juce::Component* hComps[] = { &controlPanel.leftWorkspaceAnchor, controlPanel.horizontalResizer.get(), &controlPanel.directoryRoutingView };
    controlPanel.horizontalLayoutManager.layOutComponents(hComps, 3, 
        fullBounds.getX(), fullBounds.getY(), fullBounds.getWidth(), fullBounds.getHeight(), false, true);

    // 2. Adjust right side for Exit Button
    auto rightBounds = controlPanel.directoryRoutingView.getBounds();
    auto exitRow = rightBounds.removeFromTop(height + margin * 2);
    controlPanel.exitButton.setBounds(exitRow.getRight() - Config::Layout::buttonWidth - margin, 
                                      exitRow.getY() + margin, 
                                      Config::Layout::buttonWidth, height);
    controlPanel.directoryRoutingView.setBounds(rightBounds); 

    // 3. Lay out left side using a single, globally padded internal container
    auto bounds = controlPanel.leftWorkspaceAnchor.getBounds().reduced(margin);

    layoutTopRowButtons(bounds, height);
    layoutCutControls(bounds, height);
    layoutWaveformAndStats(bounds);
}

void LayoutManager::layoutTopRowButtons(juce::Rectangle<int> &bounds, int height) {
    auto topRow = bounds.removeFromTop(height);
    if (controlPanel.topBarView != nullptr)
        controlPanel.topBarView->setBounds(topRow);
        
    bounds.removeFromTop(Config::Layout::windowBorderMargins); // 15px gap between rows
}

void LayoutManager::layoutCutControls(juce::Rectangle<int> &bounds, int height) {
    const float unit = Config::UI::WidgetUnit;
    const int spacing = (int)Config::UI::GroupSpacing;
    auto cutRow = bounds.removeFromTop(height);

    const int stripWidth = (int)((Config::UI::CutButtonWidthUnits * 2 + Config::UI::TimerWidthUnits + 
                                  Config::UI::ResetButtonWidthUnits * 2 + Config::UI::ThresholdWidthUnits) * unit) + (spacing * 5);

    if (controlPanel.inStrip != nullptr)
        controlPanel.inStrip->setBounds(cutRow.removeFromLeft(stripWidth));

    if (controlPanel.outStrip != nullptr)
        controlPanel.outStrip->setBounds(cutRow.removeFromRight(stripWidth));

    if (controlPanel.getCutLengthStrip() != nullptr) {
        controlPanel.getCutLengthStrip()->setBounds(cutRow.withSizeKeepingCentre(Config::Layout::cutLengthStripWidth, height));
    }
    
    bounds.removeFromTop(Config::Layout::windowBorderMargins); // 15px gap before waveform
}

void LayoutManager::layoutWaveformAndStats(juce::Rectangle<int> &bounds) {
    // The bounds are now perfectly prepared. Cache them.
    controlPanel.layoutCache.contentAreaBounds = bounds;
    controlPanel.getDependencies().getStatsPresenter().layoutWithin(bounds);
}

void LayoutManager::layoutWaveformArea() {
    auto* wcv = controlPanel.getWaveformCanvasView();
    if (wcv == nullptr) return;

    const int margin = Config::Layout::windowBorderMargins;
    auto contentArea = controlPanel.layoutCache.contentAreaBounds;
    auto fullLeftArea = controlPanel.leftWorkspaceAnchor.getBounds();

    // 1. Math Phase: Execute Vertical Layout
    juce::Component* vComps[] = { &controlPanel.waveformLayoutAnchor, controlPanel.verticalResizer.get(), &controlPanel.fileQueuePlaceholder };
    controlPanel.verticalLayoutManager.layOutComponents(vComps, 3,
        fullLeftArea.getX(), contentArea.getY(), fullLeftArea.getWidth(), contentArea.getHeight(), true, true);

    auto waveAnchorBounds = controlPanel.waveformLayoutAnchor.getBounds();
    juce::Rectangle<int> waveVisualBounds;

    // 2. Visual Phase: Mode-specific padding
    if (controlPanel.getSessionState().getViewMode() == AppEnums::ViewMode::Overlay) {
        waveVisualBounds = waveAnchorBounds;
        waveVisualBounds.setTop(0); // Stretch flush to ceiling
    } else {
        // Classic Mode: Trim the sides and the bottom to create uniform padding around the resizer
        waveVisualBounds = waveAnchorBounds.withTrimmedLeft(margin).withTrimmedRight(margin).withTrimmedBottom(margin);
    }

    // Pad the file queue so it perfectly aligns with the UI columns above it
    auto queueBounds = controlPanel.fileQueuePlaceholder.getBounds();
    controlPanel.fileQueuePlaceholder.setBounds(queueBounds.withTrimmedLeft(margin).withTrimmedRight(margin).withTrimmedTop(margin));

    // 3. Anchor playback timers securely inside the visual bounds
    if (auto* ptv = controlPanel.getPlaybackTimeView()) {
        const int textHeight = Config::Layout::Text::playbackHeight;
        ptv->setBounds(waveVisualBounds.getX(), waveVisualBounds.getBottom() - textHeight - margin, 
                       waveVisualBounds.getWidth(), textHeight);
    }

    wcv->setBounds(waveVisualBounds);
    controlPanel.layoutCache.waveformBounds = waveVisualBounds;
}
