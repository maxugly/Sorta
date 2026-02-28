/**
 * @file PresenterCore.cpp
 */

#include "Presenters/PresenterCore.h"
#include "UI/ControlPanel.h"

#include "Presenters/BoundaryLogicPresenter.h"
#include "Presenters/BoundaryLockPresenter.h"
#include "Presenters/MarkerLockPresenter.h"
#include "Presenters/ControlButtonsPresenter.h"
#include "Presenters/ControlStatePresenter.h"
#include "Presenters/CutButtonPresenter.h"
#include "Presenters/CutResetPresenter.h"
#include "Presenters/PlaybackRepeatController.h"
#include "Presenters/PlaybackTextPresenter.h"
#include "Presenters/CutLengthPresenter.h"
#include "Presenters/RepeatButtonPresenter.h"
#include "Presenters/SilenceDetectionPresenter.h"
#include "Presenters/StatsPresenter.h"
#include "Presenters/ZoomPresenter.h"
#include "Presenters/MatrixPresenter.h"
#include "Presenters/VolumePresenter.h"
#include "Presenters/CutPresenter.h"
#include "Presenters/SilenceThresholdPresenter.h"
#include "Presenters/KeybindPresenter.h"
#include "Presenters/HintPresenter.h"
#include "Presenters/FpsPresenter.h"
#include "Presenters/ThemePresenter.h"
#include "UI/Views/TopBarView.h"
#include "UI/Views/WaveformCanvasView.h"
#include "UI/Views/CutLayerView.h"

PresenterCore::PresenterCore(ControlPanel &cp) : owner(cp) {
    playbackRepeatController = std::make_unique<PlaybackRepeatController>(owner.getAudioPlayer(), owner.getSessionState());
    
    statsPresenter = std::make_unique<StatsPresenter>(owner);
    silenceDetectionPresenter = std::make_unique<SilenceDetectionPresenter>(owner, owner.getSessionState(), owner.getAudioPlayer());
    
    playbackTextPresenter = std::make_unique<PlaybackTextPresenter>(owner);
    playbackTextPresenter->initialiseEditors();
    
    cutLengthPresenter = std::make_unique<CutLengthPresenter>(owner);
    cutLengthPresenter->initialiseEditors();
    
    repeatButtonPresenter = std::make_unique<RepeatButtonPresenter>(owner, owner.getAudioPlayer(), owner.getSessionState());
    if (auto* ts = owner.getTransportStrip())
        repeatButtonPresenter->initialiseButton(ts->getRepeatButton());

    boundaryLogicPresenter = std::make_unique<BoundaryLogicPresenter>(
        owner, owner.getInStrip()->getTimerEditor(), owner.getOutStrip()->getTimerEditor());
    boundaryLogicPresenter->initialiseEditors();

    boundaryLockPresenter = std::make_unique<BoundaryLockPresenter>(owner, owner.getSessionState());
    markerLockPresenter = std::make_unique<MarkerLockPresenter>(owner, owner.getSessionState());

    buttonPresenter = std::make_unique<ControlButtonsPresenter>(owner);
    buttonPresenter->initialiseAllButtons();

    cutPresenter = std::make_unique<CutPresenter>(
        owner, owner.getSessionState(), owner.waveformCanvasView->getCutLayerView(),
        owner.getInteractionCoordinator(),
        owner.getPlaybackTimerManager());

    cutButtonPresenter = std::make_unique<CutButtonPresenter>(owner);
    cutResetPresenter = std::make_unique<CutResetPresenter>(owner);
    controlStatePresenter = std::make_unique<ControlStatePresenter>(owner);
    zoomPresenter = std::make_unique<ZoomPresenter>(owner);
    matrixPresenter = std::make_unique<MatrixPresenter>(owner);

    silenceThresholdPresenter = std::make_unique<SilenceThresholdPresenter>(
        owner, owner.getInStrip()->getThresholdEditor(), owner.getOutStrip()->getThresholdEditor());

    keybindPresenter = std::make_unique<KeybindPresenter>(owner);

    if (auto* tbv = owner.getTopBarView())
        volumePresenter = std::make_unique<VolumePresenter>(tbv->getVolumeView(), owner.getSessionState());

    hintPresenter = std::make_unique<HintPresenter>(owner, owner.getHintView());
    fpsPresenter = std::make_unique<FpsPresenter>(owner, owner.getFpsView());
    themePresenter = std::make_unique<ThemePresenter>(owner);
}

PresenterCore::~PresenterCore() = default;
