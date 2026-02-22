# 🛠️ AUDIOFILER Engineering Specification
### The Single Authority for All Contributors
**v3.1 — Living Document**

Before writing a single line of code, any contributor must answer YES to these three questions:
1. Does my change comply with the Four Laws?
2. Does my change pass the Done-State tests?
3. Does my change keep file sizes within the size budget?

A single **NO** is a blocker. Stop and refactor first.

---

## 🏗️ I. The Four Architectural Laws
Every proposed change must be validated against these four non-negotiable laws. Any code that violates them is incorrect by definition.

### **1. SoC (Separation of Concerns)**
*   **The Law**: Divide the software into distinct sections where each section addresses a single concern.
*   **Audio**: All file-reading and playback logic lives exclusively in `AudioPlayer`.
*   **State**: All settings and cross-file metadata live exclusively in `SessionState`.
*   **Layout**: All component positioning lives exclusively in `LayoutManager` or `resized()` methods.

### **2. SRP (Single Responsibility Principle)**
*   **The Law**: A class has one, and only one, reason to change.
*   **The Test**: Can you describe what the file does without using the word "and"? If not, split it.
*   **Size Signal**: If a file exceeds 400–500 lines, it has likely violated SRP and must be decomposed.

### **3. MVP (Model-View-Presenter)**
*   **The Law**: Decouple data (Model) from representation (View) via a middleman (Presenter).
*   **Model**: `SessionState` and `FileMetadata` have zero knowledge that a UI exists.
*   **View**: Dumb JUCE components (e.g., `WaveformView`). Only `paint()` and `resized()`. No business logic.
*   **Presenter**: Logic bricks (e.g., `TransportPresenter`) that act as the glue between Model and View.
*   **Peer Rule**: Presenters must be peers. One Presenter must never own another; they communicate via `SessionState` or shared Listeners only.

### **4. DRY (Don't Repeat Yourself)**
*   **The Law**: Every piece of knowledge has a single, unambiguous representation in the system.
*   **Centralization**: All hex color values, pixel widths, and user-facing UI strings must be centralized (e.g., in `Config.h`). Never hardcode a color or a string literal in a `.cpp` file.

---

## 🚦 II. Done-State Contracts
Each major feature area has a binding contract. A feature is not "done" until it passes all checks.

### **Contract 1 — MVP Structural Migration**
*   **Goal**: Move from a Stacked Hierarchy to Peer-to-Peer communication. `ControlPanel.h` must contain only layout code.
*   **Done State**: Every UI feature has a dedicated Presenter (the Brain) and a Passive View (the Display). All UI elements are moved out of `ControlPanel.h` into their respective Presenter-managed View groups.

### **Contract 2 — Total Naming Purity (The No-Loop Zone)**
*   **Goal**: The word "Loop" must not appear in the source code except where it refers to a literal programmatic loop.
*   **Done State**: A case-insensitive global search for "Loop" in the `Source/` folder returns zero results. Terminology must strictly use Boundaries, Regions, Cut Mode, and Repeat.

### **Contract 3 — Metadata Resource Pool**
*   **Goal**: `SessionState` is a high-speed Data Hub that serves multiple files, not only the currently active one.
*   **Done State**: The `metadataCache` map is the Single Source of Truth. The background worker writes to this map, and the `CutPresenter` reads from it.

### **Contract 4 — Thread-Safe Air Gaps**
*   **Goal**: Zero direct contact between the Background Worker and any UI component.
*   **Done State**: The `SilenceAnalysisWorker` communicates exclusively via `juce::MessageManager::callAsync`. It never touches the `AudioPlayer` mutex and creates its own private `AudioFormatReader` for each file.

---

## 📏 III. File Size Budget
File length is a proxy for responsibility. Long files hide bugs and signal architectural violations. Exceeding the target budget is a **refactor trigger**, not a warning.

*   **`ControlPanel.cpp`**: Target **< 150 lines**. Must delegate 100% of logic to sub-presenters, acting only as a layout container.
*   **Mouse Handlers**: Separate into specific handlers (e.g., `MarkerMouseHandler` and `WaveformMouseHandler`). Keep one type of interaction per file.
*   **Presenters**: Target **< 200-300 lines**. Offload helper math/utilities or time-string formatting to dedicated Utils files.
*   **Views**: Target **< 100 lines**. Views contain only `paint()` and `resized()`. Move all logic upstream.

---

## ⚡ IV. Operational Constraints

### **⛔ The Observer Law — No Polling**
*   **Forbidden**: Using `timerCallback()` to check whether a state value has changed.
*   **Required**: Use the Observer Pattern. All components that need to react to state changes must implement the `SessionState::Listener` interface and register themselves.

### **⛔ The Threading Law — Anti-Freeze Protocol**
*   **Private Reader**: `SilenceAnalysisWorker` must instantiate its own independent `AudioFormatReader`. It must never use the `AudioPlayer` reader.
*   **Async Return**: Results are pushed to the Message Thread via `juce::MessageManager::callAsync` only. Never block the UI thread waiting for a background result.

---

## 📦 V. Modularity and Portability

### **The Lego Standard**
*   Every feature is a self-contained brick that can be wired in or out.
*   Plan every new feature as a separate class before integrating it. Parent components own children via `std::unique_ptr` only.

### **Headless Portability**
*   The engine must be fully testable without a GUI.
*   Wrap all GUI-dependent code in `#if !defined(JUCE_HEADLESS)` guards so the core logic can be tested in a terminal-only environment.

---

## 🗺️ VI. Component Ownership Map
The application follows a strict top-down ownership hierarchy.

*   **`MainComponent`**: Root Shell. Owns and constructs all top-level components.
*   **`SessionState`**: The Brain. Holds all settings and acts as the Radio Station broadcast source.
*   **`AudioPlayer`**: Audio Engine. Strictly handles file reading and playback.
*   **`ControlPanel`**: Visual Container. Acts only as a parent shell for sub-views and presenters.
*   **Presenters & Views**: Logic Bricks and Display Canvas. 

---

## 📈 VII. Documentation Requirements
Every commit must satisfy the Doxygen parser to maintain the Interactive Study Map:

*   **`@file`**: Required at the top of every header file.
*   **`@ingroup`**: Must assign the class to a logical module.
*   **`@see`**: Mandatory for cross-referencing peer Presenters or related Models.

---

## 🛡️ VIII. Contributor Guardrails (Pre-commit Checklist)
Run through every item before submitting any change. A single **NO** is a blocker.

*   [ ] Can I describe what this file does without using the word "and"?
*   [ ] Does this file stay within its line-count budget?
*   [ ] Is any logic inside a `paint()` or `resized()` method? *(Must be No)*
*   [ ] Is any hex color or pixel width hardcoded in a `.cpp` file? *(Must be No)*
*   [ ] Does any `timerCallback()` poll for a state change? *(Must be No)*
*   [ ] Does the word "Loop" appear anywhere in Source/ (case-insensitive)? *(Must be No)*
*   [ ] Does the background worker touch any UI pointer directly? *(Must be No)*
*   [ ] Do all Presenters communicate via `SessionState` rather than direct references?
*   [ ] Is every new feature class wired in via `std::unique_ptr`?

---

## 📖 IX. Authoritative Terminology
Use only these exact terms for these specific features:

*   **Boundary / Region / Cut**: Describing a marked audio segment.
*   **Cut Mode**: The mode for setting region boundaries.
*   **Repeat Enabled**: The playback cycling flag.
*   **Repeat**: The transport button for cycling playback.
*   **Cycle / Repeat**: The act of playback returning to start.
