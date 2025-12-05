# Task completion checklist
- Run clang-format on touched C++ files (`clang-format -i src/<file>.{cpp,hpp}`) to match .clang-format.
- Build Debug to ensure DLL compiles: `cmake --build build --config Debug` (configure first if needed). Optionally build Release for shipping.
- Manual smoke (no automated tests yet): load DLL in target app, confirm logger shows hook init, menu toggles with Insert, hooks (WndProc/renderer) init cleanly, and features like PlayerESP respect settings; capture Logger output or crash dumps if issues.
- Call out dependency bumps and whether clean CMake configure is required; ensure new features register with FeatureManager and defaults in SettingsManager; remove/disable hooks on shutdown.
