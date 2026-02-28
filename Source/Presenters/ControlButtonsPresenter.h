

#ifndef AUDIOFILER_CONTROLBUTTONSPRESENTER_H
#define AUDIOFILER_CONTROLBUTTONSPRESENTER_H

class ControlPanel;

/**
 * @file ControlButtonsPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for managing transport and navigation button actions.
 */

class ControlPanel;

/**
 * @class ControlButtonsPresenter
 * @brief Orchestrates the command routing for primary UI buttons.
 * 
 * @details Architecturally, this presenter acts as the "glue" between the 
 *          various buttons in the ControlPanel (View) and the command 
 *          dispatch logic (Model/SessionState). It ensures that button 
 *          clicks result in the correct state mutations or audio engine 
 *          triggers.
 * 
 * @see SessionState
 * @see ControlPanel
 */
class ControlButtonsPresenter final {
  public:
    explicit ControlButtonsPresenter(ControlPanel &ownerPanel);

    void initialiseAllButtons();

    void refreshStates();

  private:
    void initialiseOpenButton();

    void initialisePlayStopButton();

    void initialiseStopButton();

    void initialiseModeButton();

    void initialiseChannelViewButton();

    void initialiseExitButton();
    void initialiseStatsButton();
    void initialiseAutoplayButton();
    void initialiseCutButton();
    void initialiseMarkerButtons();

  private:
    ControlPanel &owner;
};

#endif
