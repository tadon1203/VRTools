# Style and conventions
- Formatting: .clang-format (LLVM base); 4-space indent; 120-column limit; include blocks regrouped (system C headers, C++ std, third-party like imgui/fmt/json, then project); left pointer alignment; CRLF endings; prefer fmt for formatting/logging.
- Naming: Classes/types PascalCase; methods/functions camelCase; constants uppercase snake; filenames mirror primary class.
- Code organization: Keep headers light; order includes as system, standard, third-party, project. Use Utils/JsonConverters for JSON conversions. Keep hooks/features registered via FeatureManager and default settings via SettingsManager.
- Entry/menu behavior: Menu toggled with Insert key; ensure hooks disabled on shutdown for stable DLL unload (per AGENTS guidelines).
