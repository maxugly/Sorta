#include "UI/Components/CutLengthStrip.h"
#include "Core/AppEnums.h"
#include "Utils/Config.h"

CutLengthStrip::CutLengthStrip() {
    addAndMakeVisible(lengthEditor);
    lengthEditor.applyStandardStyle();
    lengthEditor.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Left);

    addAndMakeVisible(lockButton);
    lockButton.setButtonText(Config::Labels::lockUnlocked);
    lockButton.setClickingTogglesState(true);
    lockButton.getProperties().set("GroupPosition", (int)AppEnums::GroupPosition::Right);
}

void CutLengthStrip::resized() {
    auto b = getLocalBounds();
    const int lockWidth = (int)(Config::UI::ResetButtonWidthUnits * Config::UI::WidgetUnit);
    lockButton.setBounds(b.removeFromRight(lockWidth));
    lengthEditor.setBounds(b);
}

void CutLengthStrip::paint(juce::Graphics& g) {
    juce::ignoreUnused(g);
}
