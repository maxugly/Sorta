

#include "Presenters/CutResetPresenter.h"

#include "Core/AudioPlayer.h"
#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"

CutResetPresenter::CutResetPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
}

void CutResetPresenter::resetIn() {
    owner.getSessionState().setCutIn(0.0);
    owner.getBoundaryLogicPresenter().ensureCutOrder();
    owner.getBoundaryLogicPresenter().refreshLabels();
    owner.getSessionState().setAutoCutInActive(false);
    owner.repaint();
}

void CutResetPresenter::resetOut() {
    owner.getSessionState().setCutOut(owner.getAudioPlayer().getThumbnail().getTotalLength());
    owner.getBoundaryLogicPresenter().ensureCutOrder();
    owner.getBoundaryLogicPresenter().refreshLabels();
    owner.getSessionState().setAutoCutOutActive(false);
    owner.repaint();
}
