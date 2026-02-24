

#ifndef AUDIOFILER_CONTROLBUTTONSPRESENTER_H
#define AUDIOFILER_CONTROLBUTTONSPRESENTER_H

class ControlPanel;

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
