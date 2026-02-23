

#include "Presenters/CutResetPresenter.h"

#include "Core/AudioPlayer.h"
#include "UI/ControlPanel.h"
#include "Utils/Config.h"
#include "Workers/SilenceDetector.h"

CutResetPresenter::CutResetPresenter(ControlPanel &ownerPanel) : owner(ownerPanel) {
}

void CutResetPresenter::resetIn() {
    owner.setCutInPosition(0.0);
    owner.ensureCutOrder();
    owner.updateCutButtonColors();
    owner.refreshLabels();
    owner.getSessionState().setAutoCutInActive(false);
    owner.repaint();
}

void CutResetPresenter::resetOut() {
    owner.setCutOutPosition(owner.getAudioPlayer().getThumbnail().getTotalLength());
    owner.ensureCutOrder();
    owner.updateCutButtonColors();
    owner.refreshLabels();
    owner.getSessionState().setAutoCutOutActive(false);
    owner.repaint();
}
