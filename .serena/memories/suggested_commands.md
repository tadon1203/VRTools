# Suggested commands (Windows PowerShell)
- Configure Debug build dir: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
- Build Debug: `cmake --build build --config Debug`
- Build Release: `cmake --build build --config Release`
- Build only VRTools target: `cmake --build build --config Debug --target VRTools`
- (Future) tests placeholder: `ctest --test-dir build`
- Format touched C++ files: `clang-format -i src/<file>.{cpp,hpp}` (LLVM base, 4-space, 120 col)
- Fast search files/content: `rg --files` / `rg <pattern>`
- Basic filesystem cmds (PowerShell): `Get-ChildItem` (dir), `Set-Location <path>`, `Get-Content <file>`
