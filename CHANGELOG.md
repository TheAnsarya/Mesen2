# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased] - 2025-11-17

### Added
- Thread-safe buffered socket communication for DiztinGUIsh integration
- Comprehensive documentation for DiztinGUIsh integration architecture
- Socket class methods: `BufferedSend()` and `SendBuffer()`
- Mutex protection for concurrent socket operations
- Proper include path configuration across all project files

### Fixed
- **Critical**: Socket method linker errors (LNK2019) preventing DiztinGUIsh communication
- **Critical**: DiztinguishBridge Log method compilation errors (C2039)
- **Build**: Empty AdditionalIncludeDirectories in all project configurations
- **Build**: Missing mutex header includes causing std::mutex compilation failures
- **Quality**: Inconsistent newline endings in project files

### Changed
- Enhanced Socket class with thread-safe buffering capabilities
- Updated all project files with proper include paths (../)
- Standardized project file formatting across solution

### Technical Details
- **Compiler**: Visual Studio 2026 Preview (MSBuild 18.1.0-preview-25527-05)
- **Platform Toolset**: v145
- **C++ Standard**: C++17 (maintained)
- **Architecture**: x64

### Build Verification
✅ All C++ projects compile successfully:
- SevenZip.lib
- Utilities.lib  
- Lua.lib
- Core.lib
- Windows.lib
- MesenCore.dll (InteropDLL)
- PGOHelper.exe

### Integration Impact
- Enables real-time SNES disassembly through DiztinGUIsh
- Thread-safe communication prevents data corruption
- Zero performance impact when DiztinGUIsh not connected
- Maintains full backward compatibility

---

## Previous Versions

*Historical changelog entries would be documented here for production releases.*