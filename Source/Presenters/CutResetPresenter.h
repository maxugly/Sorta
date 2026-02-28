

#ifndef AUDIOFILER_CUTRESETPRESENTER_H
#define AUDIOFILER_CUTRESETPRESENTER_H

class ControlPanel;

/**
 * @file CutResetPresenter.h
 * @Source/Core/FileMetadata.h
 * @ingroup UI
 * @brief Presenter for resetting cut boundaries to file extremities.
 */

class ControlPanel;

/**
 * @class CutResetPresenter
 * @brief Handles the logic for reverting cut points to their default (start/end) positions.
 * 
 * @details Architecturally, this presenter acts as a specialized command 
 *          dispatcher that interacts with the SessionState (Model) to 
 *          clear user-defined boundaries. It allows the View to remain 
 *          ignorant of the mathematical definition of "reset."
 * 
 * @see SessionState
 * @see ControlPanel
 */
class CutResetPresenter {
  public:
    explicit CutResetPresenter(ControlPanel &ownerPanel);

    void resetIn();

    void resetOut();

  private:
    ControlPanel &owner;
};

#endif
