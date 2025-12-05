# Repository Guidelines

## Project Structure & Module Organization
- `src/Core`: entry point (`dllmain.cpp`, `Core`) bootstraps logging, hooks, settings, menu toggle, and notifications.
- `src/Hooking`: hook definitions (`GameHooks`, `WndProcHook`, `HookManager`) enabling DX11/Win32 patches.
- `src/Features`: feature interfaces and modules (ESP, registration) plus settings wiring.
- `src/HUD` and `src/UI`: in-game overlay components, notifications, and ImGui menus (menu opens with [INSERT]).
- `src/Rendering`: D3D11 renderer setup feeding ImGui; `src/Input`: input routing and keybind registration.
- `src/SDK`: Unity/Il2Cpp/VRChat shims; `src/Utils`: math, pattern scan, and rendering helpers. CMake builds the VRTools shared library.

## Build, Test, and Development Commands
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` configures dependencies via FetchContent.
- `cmake --build build --config Debug` builds the VRTools DLL for debugging; use `Release` for shipping.
- `cmake --build build --config Debug --target VRTools` rebuilds only the library target when multiple are present.
- `ctest --test-dir build` reserved for future automated tests (none today).
- `c2p.bat` exports source to a prompt for reviews; not part of the build.

## Coding Style & Naming Conventions
- `.clang-format` (LLVM base, 4-space indent, 120 cols, regrouped includes). Run `clang-format -i src/<file>.{cpp,hpp}` before committing.
- Classes/types use PascalCase (`FeatureManager`); methods/functions camelCase; constants uppercase snake. File names mirror the primary class.
- Favor `fmt` for formatting/logging and `nlohmann::json` converters in `Utils/JsonConverters.hpp`. Keep headers light and order includes as system, standard, third-party, then project.

## Testing Guidelines
- No automated suite yet; perform manual smoke: build Debug, load in target app, confirm logs show hook init, menu toggles with INSERT, hooks (`WndProc`, renderer) initialize cleanly, and features like `PlayerESP` respect settings.
- Note scenarios tested in PRs and attach `Logger` output or crash dumps when reporting failures.

## Commit & Pull Request Guidelines
- Follow the Conventional Commit style seen in history (`feat: ...`, `refactor: ...`, `fix: ...`). Keep commits focused and explain hook/SDK offsets in the body when relevant.
- PRs should describe scope, build type used, and manual test notes; link issues. Add screenshots or short clips for UI/HUD/menu changes.
- Call out dependency bumps (fmt, imgui, minhook) and whether a clean CMake configure is needed. Ensure new features register with `FeatureManager` and default settings land in `SettingsManager`. Remove or disable hooks on shutdown to keep DLL unload stable.
