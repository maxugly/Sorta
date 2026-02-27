#include "ThemePresenter.h"
#include "UI/Views/TopBarView.h"
#include "Utils/Config.h"

ThemePresenter::ThemePresenter(ControlPanel& cp) : owner(cp) {
    if (auto* tbv = owner.getTopBarView()) {
        tbv->themeSelector.addListener(this);
        scanThemes();
        tbv->themeUpButton.onClick = [this] { cycleTheme(-1); };
        tbv->themeDownButton.onClick = [this] { cycleTheme(1); };
    }
}

ThemePresenter::~ThemePresenter() {
    if (auto* tbv = owner.getTopBarView())
        tbv->themeSelector.removeListener(this);
}

void ThemePresenter::scanThemes() {
    auto themesDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(".config/audiofiler/themes");
    themeFiles = themesDir.findChildFiles(juce::File::findFiles, false, "*.conf");

    if (auto* tbv = owner.getTopBarView()) {
        tbv->themeSelector.clear();
        int id = 1;
        for (const auto& file : themeFiles) {
            tbv->themeSelector.addItem(file.getFileNameWithoutExtension(), id++);
        }
        
        bool found = false;
        for (int i = 0; i < tbv->themeSelector.getNumItems(); ++i) {
            if (themeFiles[i].getFileName() == Config::Advanced::currentTheme) {
                tbv->themeSelector.setSelectedItemIndex(i, juce::dontSendNotification);
                found = true;
                break;
            }
        }
        if (!found) tbv->themeSelector.setText("Select Theme...", juce::dontSendNotification);
    }
}

void ThemePresenter::comboBoxChanged(juce::ComboBox* comboBox) {
    if (auto* tbv = owner.getTopBarView()) {
        if (comboBox == &tbv->themeSelector) {
            int index = comboBox->getSelectedItemIndex();
            if (index >= 0 && index < themeFiles.size()) {
                Config::loadTheme(themeFiles[index]);
                Config::saveCurrentTheme(themeFiles[index].getFileName());
                owner.refreshThemeLive();
            }
        }
    }
}

void ThemePresenter::cycleTheme(int delta) {
    if (themeFiles.isEmpty()) return;
    if (auto* tbv = owner.getTopBarView()) {
        int currentIndex = tbv->themeSelector.getSelectedItemIndex();
        int nextIndex = (currentIndex + delta + themeFiles.size()) % themeFiles.size();
        tbv->themeSelector.setSelectedItemIndex(nextIndex, juce::sendNotification);
    }
}
