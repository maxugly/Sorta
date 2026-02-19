Audiofiler Architectural Map (v2.1)
1. The Core Hierarchy (Ownership)
The application follows a strict top-down ownership model to prevent memory leaks and dangling references.

MainComponent: The Root Shell.

SessionState: The "Brain" (Holds all settings and cross-file metadata).

AudioPlayer: The "Audio Engine" (Strictly handles file reading and playback).

ControlPanel: The "Visual Container" (Acts only as a parent for sub-views and presenters).

Presenters: Logic sub-modules (e.g., CutResetPresenter, StatsPresenter) that manage specific UI behavior.

2. Peer-to-Peer MVP Architecture (Strict Rules)
To make the code "study-able," we are moving away from the ControlPanel as a "God Object".

The Model: SessionState and FileMetadata. They do not know the UI exists.

The View: Components like WaveformView and ZoomView. They only handle paint() and do not contain business logic.

The Presenter: Objects like TransportPresenter.

Rule: Presenters are the "glue".

Rule: Presenters should be peer-to-peer. One presenter should not own another; they should communicate via the SessionState or shared listeners.

Refactor Goal: Strip all juce::TextButton and juce::TextEditor members out of ControlPanel.h and move them into specialized "View" groups managed by their respective Presenters.

3. The State Handshake (Broadcast Model)
We use a Broadcasting Pattern to keep the UI and Engine in sync without circular dependencies.

The Radio Station: SessionState acts as the single source of truth.

The Tune-In: AudioPlayer and ControlPanel (via its presenters) inherit from SessionState::Listener.

The Workflow:

User interacts with a View (UI).

The Presenter handles the event and updates SessionState.

SessionState broadcasts a cutPreferenceChanged message.

The AudioPlayer receives the broadcast and updates the playback engine instantly.

4. Background Safety (Private Reader Strategy)
To prevent UI freezing during heavy analysis, the SilenceAnalysisWorker follows these threading rules:

Private Reader: The worker must create its own independent AudioFormatReader for the file.

Lock Avoidance: It must not touch the AudioPlayer mutex.

Asynchronous Return: Use juce::MessageManager::callAsync to push results back to SessionState once the background thread finishes.

5. Critical "Don't Do" Rules (The Student/Agent Guardrails)
Rule 1: No God Objects. If a header file has more than 10 std::unique_ptr sub-components, it needs to be broken down into smaller, peer-to-peer groups.

Rule 2: No Logic in Views. If you see a math calculation in a paint() or resized() method, move it to a CoordinateMapper or a Presenter.

Rule 3: Listener Authority. Never poll variables in a timerCallback. If you need to know a value changed, implement a Listener interface.

Rule 4: Doxygen Documentation. Every new class must include @ingroup and @see tags to maintain the interactive study map.
