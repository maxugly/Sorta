# Audiofiler Verification Report

## 1. Header Audit: Source/FocusManager.h

**Status**: Verified.
The file `Source/FocusManager.h` correctly implements the requested Doxygen headers, including `@file`, `@class`, `@brief`, and the explicit **Hierarchy of Intent**.

**Current Content Snippet:**
```cpp
/**
 * @class FocusManager
 * @brief Centralizes the logic for determining "what matters right now" in the
 * UI.
 *
 * The FocusManager implements a strict **Hierarchy of Intent** to resolve
 * conflicts between multiple potential sources of focus (e.g., playback running
 * while user hovers a text box).
 *
 * **Hierarchy of Intent (Highest to Lowest Priority):**
 * 1. **Dragging**: The user is actively dragging a handle (Cut In/Out).
 *    - This overrides everything else. If you are moving a marker, we show that
 * marker's time.
 * 2. **Scrubbing**: The user is manually scrubbing the timeline (MouseManual).
 *    - Example: Right-click + Drag on the waveform.
 * 3. **Hovering**: The user is hovering over a specific control.
 *    - Example: Mouse over the "Cut In" timer box highlights that time.
 * 4. **Playback**: The default state.
 *    - If no user interaction is happening, we follow the playhead.
 */
```

## 2. Granularity Audit: Flagged Functions (> 50 Lines)

The following functions exceed the 50-line limit suggested for "Lego-brick" separation of concerns. These are candidates for extraction or refactoring.

**Top Offenders:**
1.  **WaveformView::drawCutModeOverlays** (175 lines)
    - *Suggestion*: Extract to `CutOverlayView` class or helper.
2.  **MouseHandler::mouseDown** (143 lines)
    - *Suggestion*: Split logic by `PlacementMode` (e.g., `handleCutPlacementClick`, `handleSeekClick`).
3.  **WaveformView::drawZoomPopup** (138 lines)
    - *Suggestion*: Extract to `ZoomPopupView`.
4.  **MouseHandler::mouseDrag** (107 lines)
    - *Suggestion*: Separate scrubbing logic from drag-seeking logic.
5.  **WaveformView::drawMouseCursorOverlays** (99 lines)
    - *Suggestion*: Extract to `CursorOverlayView`.
6.  **SilenceAnalysisWorker::detectInSilence** (79 lines)
    - *Suggestion*: Extract core algorithm to `SilenceAlgorithm` helper.
7.  **SilenceAnalysisWorker::detectOutSilence** (79 lines)
    - *Suggestion*: Same as above (unify logic?).
8.  **WaveformView::drawWaveform** (65 lines)
    - *Suggestion*: Potentially fine, but could extract channel repeat logic.
9.  **MouseHandler::mouseUp** (63 lines)
    - *Suggestion*: Extract state reset logic.
10. **RepeatPresenter::mouseWheelMove** (61 lines)
    - *Suggestion*: Extract scrolling vs zooming logic.

## 3. Next Steps
Review `DOCS_MAP.md` for the architectural overview.
