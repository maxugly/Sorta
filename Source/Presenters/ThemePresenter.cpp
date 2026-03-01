#include "ThemePresenter.h"
#include "UI/Views/TopBarView.h"
#include "UI/Views/PlaybackTimeView.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/WaveformView.h"
#include "UI/Components/MarkerStrip.h"
#include "UI/Components/TransportStrip.h"
#include "UI/Components/CutLengthStrip.h"
#include "Presenters/StatsPresenter.h"
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
        if (!found) tbv->themeSelector.setText(Config::Labels::selectTheme, juce::dontSendNotification);
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

void ThemePresenter::applyTheme() {
    auto& lf = owner.modernLF;
    lf.setBaseOffColor(Config::Colors::Button::base);
    lf.setBaseOnColor(Config::Colors::Button::on);
    lf.setTextColor(Config::Colors::Button::text);
    lf.setTextActiveColor(Config::Colors::Button::textActive);
    lf.setColour(juce::ComboBox::backgroundColourId, Config::Colors::Button::base);
    lf.setColour(juce::ComboBox::outlineColourId, Config::Colors::Button::outline);
    lf.setColour(juce::ComboBox::textColourId, Config::Colors::Button::text);
    lf.setColour(juce::ComboBox::arrowColourId, Config::Colors::Button::text);
    lf.setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    lf.setColour(juce::TextEditor::outlineColourId, Config::Colors::Button::outline);

    if (auto* wcv = owner.getWaveformCanvasView()) wcv->getWaveformView().clearCaches();
    
    auto& stats = owner.getPresenterCore().getStatsPresenter();
    stats.getDisplay().setColour(juce::TextEditor::backgroundColourId, Config::Colors::textEditorBackground);
    stats.getDisplay().setColour(juce::TextEditor::textColourId, Config::Colors::statsText);
    stats.updateStats();

    if (auto* ptv = owner.getPlaybackTimeView()) {
        ptv->getElapsedEditor().setCustomTextColor(Config::Colors::playbackText);
        ptv->getRemainingEditor().setCustomTextColor(Config::Colors::playbackText);
        ptv->getTotalTimeEditor().setCustomTextColor(Config::Colors::totalTimeText);
    }
    
    auto setStripCols = [&](MarkerStrip* s) {
        if (!s) return;
        s->getTimerEditor().setCustomTextColor(Config::Colors::cutText);
        s->getThresholdEditor().setCustomTextColor(Config::Colors::thresholdText);
        s->getMarkerButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        s->getMarkerButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        s->getResetButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        s->getResetButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        s->getAutoCutButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        s->getAutoCutButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
        s->getLockButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        s->getLockButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
    };
    setStripCols(owner.getInStrip());
    setStripCols(owner.getOutStrip());

    if (auto* cls = owner.getCutLengthStrip()) {
        cls->getLengthEditor().setCustomTextColor(Config::Colors::cutText);
        cls->getLockButton().setColour(juce::TextButton::textColourOffId, Config::Colors::Button::text);
        cls->getLockButton().setColour(juce::TextButton::textColourOnId, Config::Colors::Button::textActive);
    }

    auto setBtn = [&](juce::TextButton& b, juce::Colour off, juce::Colour on) {
        b.setColour(juce::TextButton::textColourOffId, off);
        b.setColour(juce::TextButton::textColourOnId, on);
    };
    setBtn(owner.exitButton, Config::Colors::Button::exitText, Config::Colors::Button::exitText);

    if (auto* tbv = owner.getTopBarView()) {
        setBtn(tbv->openButton, Config::Colors::Button::text, Config::Colors::Button::textActive);
        setBtn(tbv->modeButton, Config::Colors::Button::text, Config::Colors::Button::textActive);
        setBtn(tbv->statsButton, Config::Colors::Button::text, Config::Colors::Button::textActive);
        setBtn(tbv->channelViewButton, Config::Colors::Button::text, Config::Colors::Button::textActive);
        setBtn(tbv->themeUpButton, Config::Colors::Button::text, Config::Colors::Button::textActive);
        setBtn(tbv->themeDownButton, Config::Colors::Button::text, Config::Colors::Button::textActive);

        if (auto* ts = tbv->transportStrip.get()) {
            setBtn(ts->getPlayStopButton(), Config::Colors::Button::text, Config::Colors::Button::textActive);
            setBtn(ts->getStopButton(), Config::Colors::Button::text, Config::Colors::Button::textActive);
            setBtn(ts->getAutoplayButton(), Config::Colors::Button::text, Config::Colors::Button::textActive);
            setBtn(ts->getRepeatButton(), Config::Colors::Button::text, Config::Colors::Button::textActive);
            setBtn(ts->getCutButton(), Config::Colors::Button::text, Config::Colors::Button::textActive);
        }
    }
    owner.sendLookAndFeelChange();
    owner.repaint();
}
