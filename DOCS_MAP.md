Audiofiler Architectural Map (v2.0)

1. The Core Hierarchy (Ownership)

The application follows a strict top-down ownership model to prevent memory leaks and dangling references.

MainComponent: The Root Shell.

SessionState: The "Brain" (Holds all settings/preferences).

AudioPlayer: The "Audio Engine" (Handles file reading and playback).

ControlPanel: The "Face" (All buttons, sliders, and visual feedback).

Presenters: Logic sub-modules (e.g., CutResetPresenter, StatsPresenter) that keep the ControlPanel code lean.

SilenceDetector: The logic bridge for automation.

SilenceAnalysisWorker: A background thread for heavy math.

2. The State Handshake (Push Model)

We use a Broadcasting Pattern to keep the UI and Engine in sync without circular dependencies.

The Radio Station: SessionState acts as the single source of truth.

The Tune-In: AudioPlayer and ControlPanel inherit from SessionState::Listener.

The Workflow:

User clicks a button (UI).

Presenter updates SessionState.

SessionState broadcasts a "Change" message.

AudioPlayer hears it and updates the playback engine instantly.

ControlPanel hears it and refreshes the display on its next 60Hz heartbeat.

3. The "Silence" Background Workflow (Anti-Freeze)

The SilenceAnalysisWorker is designed to process massive files without locking the UI.

The Problem (The Deadlock): Previously, the worker held a "Master Lock" on the audio reader, forcing the UI to freeze while it scanned.

The Solution (Private Reader):

The SilenceAnalysisWorker creates its own independent reader for the file.

It crunches math on its own thread without touching the AudioPlayer mutex.

When finished, it uses callAsync to push results back to SessionState.

4. Critical "Don't Do" Rules for New Eyes

Rule 1: No Polling. Never add logic to MainComponent::timerCallback to check if a variable has changed. Use a Listener instead.

Rule 2: Boundary Authority. The AudioPlayer::getNextAudioBlock is the only place where playback boundaries (In/Out) are enforced for sample-accuracy.

Rule 3: Private State. Never access cutPrefs directly. Use the getters and setters in SessionState to ensure every change is broadcasted to the engine.

Status Registry (Where we are right now)

Audio Engine: Sample-accurate, decoupled from the UI, and listens for state changes.

UI Foundation: ControlPanel is decentralized with its own timer and uses Presenters for all button logic.

Background Worker: Currently being refactored to use the "Private Reader" strategy to kill the permanent lockup.

Next Architectural Goal: Finalize the 0-9 keyboard sorting logic using this clean "SessionState $\rightarrow$ Broadcast" pipeline.

Would you like me to generate a specific "Agent Summary" file that you can keep in the repository for future AI assistance? It can act as a permanent memory for any fresh agent you bring in.

