#ifndef AUDIOFILER_THEMEPRESENTER_H
#define AUDIOFILER_THEMEPRESENTER_H

#include "UI/ControlPanel.h"

/**
 * @file ThemePresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for managing application-wide color palettes and themes.
 */

class ControlPanel;

/**
 * @class ThemePresenter
 * @brief Orchestrates the live switching of UI color schemes.
 * 
 * @details Architecturally, this presenter acts as the controller for 
 *          aesthetic state. It scans the filesystem for theme files and 
 *          coordinates with the ControlPanel (View) to apply new LookAndFeel 
 *          settings in real-time.
 * 
 * @see SessionState
 * @see ControlPanel
 */
class ThemePresenter final : public juce::ComboBox::Listener {
  public:
    explicit ThemePresenter(ControlPanel& owner);
    ~ThemePresenter() override;

    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void applyTheme();

  private:
    ControlPanel& owner;
    juce::Array<juce::File> themeFiles;

    void scanThemes();
    void cycleTheme(int delta);
};

#endif
