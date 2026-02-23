/**
 * @file PresenterCore.cpp
 */

#include "Presenters/PresenterCore.h"
#include "UI/ControlPanel.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/ControlButtonsPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/PlaybackRepeatController.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/ZoomPresenter.h"
#include "Presenters/MatrixPresenter.h"
#include "Presenters/VolumePresenter.h"
#include "Presenters/CutPresenter.h"
#include "UI/Views/TopBarView.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/CutLayerView.h"

PresenterCore::PresenterCore(ControlPanel &cp) : owner(cp) {
    playbackRepeatController = std::make_unique<PlaybackRepeatController>(owner.getAudioPlayer(), owner);
    
    // We need to notify the timer manager about the repeat controller
    owner.getPlaybackTimerManager().setRepeatController(playbackRepeatController.get());

    statsPresenter = std::make_unique<StatsPresenter>(owner);
    silenceDetectionPresenter = std::make_unique<SilenceDetectionPresenter>(owner, owner.getSessionState(), owner.getAudioPlayer());
    
    playbackTextPresenter = std::make_unique<PlaybackTextPresenter>(owner);
    playbackTextPresenter->initialiseEditors();
    
    repeatButtonPresenter = std::make_unique<RepeatButtonPresenter>(owner, owner.getAudioPlayer(), owner.getSessionState());
    if (auto* ts = owner.getTransportStrip())
        repeatButtonPresenter->initialiseButton(ts->getRepeatButton());

    boundaryLogicPresenter = std::make_unique<BoundaryLogicPresenter>(
        owner, owner.getSilenceDetector(), owner.getInStrip()->getTimerEditor(), owner.getOutStrip()->getTimerEditor());
    boundaryLogicPresenter->initialiseEditors();

    buttonPresenter = std::make_unique<ControlButtonsPresenter>(owner);
    buttonPresenter->initialiseAllButtons();

    cutPresenter = std::make_unique<CutPresenter>(
        owner, owner.getSessionState(), owner.waveformCanvasView->getCutLayerView(),
        owner.getSilenceDetector(), owner.getInteractionCoordinator(),
        owner.getPlaybackTimerManager());

    cutButtonPresenter = std::make_unique<CutButtonPresenter>(owner);
    cutResetPresenter = std::make_unique<CutResetPresenter>(owner);
    controlStatePresenter = std::make_unique<ControlStatePresenter>(owner);
    zoomPresenter = std::make_unique<ZoomPresenter>(owner);
    matrixPresenter = std::make_unique<MatrixPresenter>(owner);
    if (auto* tbv = owner.getTopBarView())
        volumePresenter = std::make_unique<VolumePresenter>(tbv->getVolumeView(), owner.getSessionState());
}

PresenterCore::~PresenterCore() = default;
