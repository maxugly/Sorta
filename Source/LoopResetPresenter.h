#ifndef AUDIOFILER_LOOPRESETPRESENTER_H
#define AUDIOFILER_LOOPRESETPRESENTER_H

class ControlPanel;

/**
 * @class LoopResetPresenter
 * @brief Handles clear-loop button behavior, keeping ControlPanel lean.
 */
class LoopResetPresenter
{
public:
    explicit LoopResetPresenter(ControlPanel& ownerPanel);

    void clearLoopIn();
    void clearLoopOut();

private:
    ControlPanel& owner;
};


#endif // AUDIOFILER_LOOPRESETPRESENTER_H
