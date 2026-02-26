#ifndef AUDIOFILER_THEMEPRESENTER_H
#define AUDIOFILER_THEMEPRESENTER_H

#include "UI/ControlPanel.h"

class ThemePresenter final : public juce::ComboBox::Listener {
  public:
    explicit ThemePresenter(ControlPanel& owner);
    ~ThemePresenter() override;

    void comboBoxChanged(juce::ComboBox* comboBox) override;

  private:
    ControlPanel& owner;
    juce::Array<juce::File> themeFiles;

    void scanThemes();
    void cycleTheme(int delta);
};

#endif
