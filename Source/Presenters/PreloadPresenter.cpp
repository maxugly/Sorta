#include "Presenters/PreloadPresenter.h"
#include "UI/ControlPanel.h"
#include "UI/Views/FileQueueView.h"

PreloadPresenter::PreloadPresenter(ControlPanel& ownerPanel, FileQueueView& view)
    : owner(ownerPanel), queueView(view) {
    owner.getSessionState().addListener(this);

    FileQueueViewState initialState;
    queueView.updateState(initialState);
}

PreloadPresenter::~PreloadPresenter() {
    owner.getSessionState().removeListener(this);
}
