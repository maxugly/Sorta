| File | Line/Method | Violation Type | Description & Recommended Fix |
|---|---|---|---|
| Source/Presenters/PresenterCore.h | Line 100 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<MarkerLockPresenter> markerLockPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 101 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<ControlButtonsPresenter> buttonPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 102 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<CutButtonPresenter> cutButtonPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 103 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<CutResetPresenter> cutResetPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 104 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<ControlStatePresenter> controlStatePresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 106 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<ZoomPresenter> zoomPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 107 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<MatrixPresenter> matrixPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 108 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<VolumePresenter> volumePresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 109 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<CutPresenter> cutPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 110 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<SilenceThresholdPresenter> silenceThresholdPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 111 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<KeybindPresenter> keybindPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 112 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<HintPresenter> hintPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 113 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<FpsPresenter> fpsPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 114 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<ThemePresenter> themePresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 93 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<StatsPresenter> statsPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 94 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<SilenceDetectionPresenter> silenceDetectionPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 95 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<PlaybackTextPresenter> playbackTextPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 96 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<CutLengthPresenter> cutLengthPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 97 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<RepeatButtonPresenter> repeatButtonPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 98 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<BoundaryLogicPresenter> boundaryLogicPresenter;`. Presenters must be peers. |
| Source/Presenters/PresenterCore.h | Line 99 | MVP | Presenter owns another presenter via std::unique_ptr: `std::unique_ptr<BoundaryLockPresenter> boundaryLockPresenter;`. Presenters must be peers. |
| Source/Presenters/StatsPresenter.h | Line 86 in resized() | MVP | Found logic in a Component method (StatsPresenter inherits Component!). Views/Components must be completely 'dumb'. |
| Source/UI/Views/CutLayerView.cpp | 167 lines | Budget (File Size) | View file exceeds 100 lines. Break down into sub-components. |
| Source/UI/Views/CutLayerView.h | 136 lines | Budget (File Size) | View file exceeds 100 lines. Break down into sub-components. |
| Source/UI/Views/DirectoryRoutingView.cpp | Line 18 | Raw Math in UI | Raw color math used (e.g., darker/brighter). Must use Config::Colors variables. |
| Source/UI/Views/DirectoryRoutingView.cpp | Line 33 | Dead Code | Empty resized() method. |
| Source/UI/Views/ZoomView.cpp | 168 lines | Budget (File Size) | View file exceeds 100 lines. Break down into sub-components. |
| Source/UI/Views/ZoomView.h | 207 lines | Budget (File Size) | View file exceeds 100 lines. Break down into sub-components. |
| Source/Utils/TimeUtils.cpp | Line 32 | Vocabulary | Forbidden term 'trim' used. Replace with authoritative term 'Autocut', 'Threshold', 'Boundary', 'Region', 'Cut Mode', or 'Repeat'. |
