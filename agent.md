Agent.md: Refactoring and Development Practices for audiofiler
Introduction
This document outlines the core principles, practices, and workflows for refactoring and developing the audiofiler JUCE-based audio application. Our approach is patient, iterative, and focused on real-world success — treating the codebase like "Lego bricks and wires" for easy understanding, maintenance, and expansion. The goal is a perfectly maintainable, coherent, best-practice C++/JUCE project that's team-friendly, extensible, and tuned for LLM assistance (e.g., short functions, descriptive names, explicit comments).
These practices draw from our conversations and refactoring journey, emphasizing modularity, testing, and gradual change to avoid bugs or "shooting ourselves in the foot."
Core Theory: Separation of Concerns
What is Separation of Concerns?
Separation of Concerns (SoC) is a fundamental software design principle that advocates dividing a program into distinct sections, each addressing a separate "concern" or responsibility. In audiofiler, this means breaking down the monolithic MainComponent (originally a "god class" handling UI, audio, events, states, etc.) into small, single-responsibility classes.

Why SoC?
Improves readability: Each file/class focuses on one thing (e.g., AudioPlayer for playback, MouseHandler for input).
Enhances maintainability: Changes in one area (e.g., adding a button) don't ripple everywhere.
Boosts testability: Small classes are easier to unit-test (e.g., mock AudioPlayer to test SilenceDetector).
Enables extensibility: New features plug in like Lego (e.g., add a PluginManager without touching UI).
LLM-friendly: Short files (<500 lines) with clear comments make AI tools (like Gemini or Jules) more effective for generation/refactoring.

How we apply SoC in audiofiler:
Single Responsibility Principle (SRP): Each class does one job (e.g., WaveformRenderer only renders visuals, KeybindHandler only handles keys).
Ownership & Delegation: Parent classes (e.g., ControlPanel) own child classes via std::unique_ptr and delegate (e.g., controlPanel->mouseHandler->mouseDown(event)).
Interfaces/Refs: Use references for access (e.g., MouseHandler takes ControlPanel& ref to get bounds, avoiding tight coupling).
Granularity: We went "super granular" (e.g., PlaybackCursorGlow for one visual effect) to make pieces like Lego — easy for a noob to see/rewire.


Example: Original MainComponent mixed audio + UI + events. Now: AudioPlayer for sound, ControlPanel for widgets, MouseHandler for input — each a brick connected by wires (methods/refs).
Bringing New Features and Code In Properly
New features must integrate without disrupting modularity. Follow this process to "bring in new code properly":

Plan the Feature as Lego:
Identify concerns: Does it touch audio (extend AudioPlayer)? UI (add to ButtonManager)? Both (new class bridging them)?
Design small: Break into 1-2 new classes/files (e.g., for "export audio", add AudioExporter owned by AudioPlayer).
Use existing hooks: Delegate via owners (e.g., add method to ControlPanel that calls new class).

One Small Addition Per Session:
Similar to refactoring: Add one class/method at a time.
Prompt LLM (e.g., Jules): "Add export feature to AudioPlayer as new method exportLoopToFile(File outFile). Preserve modularity, use Config for formats."
No big bangs — avoid touching multiple files unless necessary.

Integrate with Existing Practices:
Config.h First: Add configurable params (e.g., constexpr juce::String exportFormat = ".wav";).
Doxygen from Day 1: Document with @brief/@param/"why" comments.
Modern C++: Use smart pointers, const, RAII; avoid globals.
Delegation: Wire via refs/owners (e.g., MainComponent delegates to ControlPanel->exportButtonClicked()).
No Monoliths: If feature spans concerns, create a coordinator class (e.g., ExportCoordinator owns UI + audio logic).

Build/Test Cycle:
After code: cmake -G Ninja -B build then ninja -C build.
Test manually: Run app, verify feature (e.g., export button saves file).
Add unit test: Simple one for new class (e.g., mock AudioBuffer, assert exported file exists).
If bugs: Use Git reset; fix iteratively.

Edge Cases & Errors:
Handle failures (e.g., export fails → log to statsDisplay with errorColor).
Preserve states (e.g., new feature doesn't reset loop positions).


Example: Adding "undo loop changes" — Create LoopHistory.h/cpp (stores position stacks), owned by LoopPresenter. Add methods like pushState(), undo(); wire to new button in ButtonManager.

All Our Specific Practices
1. Configuration & Theming (The "Config.h" Standard)

Nested Semantic Structs: Group constants by Component (e.g., Waveform, Button) inside a parent struct like Layout.

The DRY (Don't Repeat Yourself) Principle: Avoid redundant naming. Use Config::Layout::Button::height instead of Config::buttonHeight.

Header/Implementation Split:

Header (.h): Use static constexpr for primitives. Use extern const for complex types like juce::Colour.

Implementation (.cpp): Define the actual values for extern objects here to avoid multiple-definition linker errors.

Derived Colors: Use semantic variables (e.g., primary) and derive UI variations using JUCE methods like withAlpha() or withMultipliedBrightness().

2. Git & Environment Health

The "Zombie" Rule: .gitignore does not untrack files already in the index. Use git rm -r --cached <path> to fix "red entries" in Lazygit for ignored folders.

Pre-commit Verification: Before committing, verify no ignored files are tracked using git ls-files -c -i --exclude-standard.

Clean Slate Alias: Maintain a shell function/alias (git-clean-index) that untracks everything and re-adds based on the current .gitignore.

3. Refactoring Workflow

One Extraction Per Session: Small steps (e.g., extract mouse to MouseHandler) to avoid LLM context collapse.

The "Jules" Pivot: If a task changes the fundamental architecture (like the flat-to-nested namespace shift), start a new job/task.

Mapping Tables: Before a global search-and-replace, require the agent to provide a mapping table (OldVariable -> NewPath) for verification.

Build-Tested Iteration: Run ninja after every header change. Use compiler errors as a checklist for the search-and-replace process.

4. Code Style & Best Practices

Doxygen Comments: @file, @class, @brief, @param, and @return are mandatory. Explain why (intent) rather than just what.

Modern C++: Use std::unique_ptr for ownership, const references for access, and RAII.

Granularity: Aim for "Lego-brick" classes. If a file exceeds 400-500 lines, it’s time to extract a sub-component.