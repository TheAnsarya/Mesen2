# 🚀 Mesen2 Modernization Roadmap

> **Branch:** `modernization` (sub-branch of `pansy-export`)
> **Baseline Tag:** `v2.0.0-pansy-phase3`
> **Started:** January 26, 2026

## 📋 Executive Summary

This modernization effort upgrades Mesen2 from .NET 8 to .NET 10, updates all dependencies to their latest versions, implements comprehensive testing, and modernizes the codebase to use current best practices and built-in libraries.

## 🎯 Goals

1. **Modern Runtime** - Upgrade to .NET 10 for latest features and performance
2. **Latest Dependencies** - Update Avalonia 11.3.1 → 11.3.11+, all NuGet packages
3. **Comprehensive Testing** - Full test coverage for critical paths
4. **Modern Libraries** - Use built-in APIs (System.IO.Hashing, etc.) instead of custom implementations
5. **Code Quality** - Modern C# patterns, nullable reference types, analyzers
6. **Lua Runtime** - Update embedded Lua to latest version

## 📊 Current State (Pre-Modernization)

| Component | Current Version | Target Version |
|-----------|-----------------|----------------|
| .NET | 8.0 | 10.0 |
| Avalonia | 11.3.1 | 11.3.11+ |
| Avalonia.AvaloniaEdit | 11.3.0 | Latest |
| Dock.Avalonia | 11.3.0.2 | Latest |
| ELFSharp | 2.17.3 | Latest |
| ReactiveUI.Fody | 19.5.41 | Latest |
| Lua (embedded) | 5.4.x | 5.4.7+ |

## 🗺️ Phases

### Phase 1: .NET 10 Migration (Priority: HIGH)

**Objective:** Update target framework from net8.0 to net10.0

#### Tasks
- [ ] Update `UI.csproj` TargetFramework to net10.0
- [ ] Update `DataBox.csproj` TargetFramework to net10.0
- [ ] Update `Mesen.Tests.csproj` TargetFramework to net10.0
- [ ] Update any RuntimeIdentifier configurations
- [ ] Fix any breaking changes from .NET 10
- [ ] Update C++ interop project if needed
- [ ] Test on Windows, Linux, macOS

#### Breaking Changes to Watch
- Any deprecated APIs from .NET 8
- JSON serialization changes
- Native AOT compatibility changes
- Trimming behavior changes

### Phase 2: Avalonia Update (Priority: HIGH)

**Objective:** Update to Avalonia 11.3.11+ with all related packages

#### Tasks
- [ ] Update Avalonia to 11.3.11
- [ ] Update Avalonia.Desktop to 11.3.11
- [ ] Update Avalonia.Controls.ColorPicker to 11.3.11
- [ ] Update Avalonia.Diagnostics to 11.3.11
- [ ] Update Avalonia.ReactiveUI to 11.3.11
- [ ] Update Avalonia.Themes.Fluent to 11.3.11
- [ ] Update Avalonia.AvaloniaEdit to latest
- [ ] Update Dock.Avalonia and Dock.Model.Mvvm to latest
- [ ] Fix any breaking changes in XAML or code-behind
- [ ] Test all UI components

#### Related Packages
- Dock.Avalonia (11.3.0.2 → latest)
- AvaloniaEdit (11.3.0 → latest)
- ReactiveUI.Fody (19.5.41 → latest)

### Phase 3: Built-in Libraries Migration (Priority: MEDIUM)

**Objective:** Replace custom implementations with modern .NET built-in libraries

#### CRC32 Migration
- [ ] Replace custom CRC32 with System.IO.Hashing.Crc32
- [ ] Update PansyExporter to use System.IO.Hashing
- [ ] Update any other CRC32 usage in codebase
- [ ] Performance comparison and validation

```csharp
// Before (custom)
private static uint CalculateCrc32(byte[] data) { ... }

// After (built-in)
using System.IO.Hashing;
var crc = Crc32.HashToUInt32(data);
```

#### Other Built-in Opportunities
- [ ] Audit for custom JSON serialization (use System.Text.Json source generators)
- [ ] Audit for custom compression (use System.IO.Compression)
- [ ] Audit for custom collections (use modern collection expressions)
- [ ] Audit for custom hash algorithms (use System.Security.Cryptography)

### Phase 4: Comprehensive Testing (Priority: HIGH)

**Objective:** Achieve high test coverage for critical components

#### Testing Infrastructure
- [ ] Complete xUnit test project setup
- [ ] Add code coverage tooling (coverlet)
- [ ] Set up CI/CD test runs
- [ ] Add integration tests

#### Test Coverage Goals
| Component | Current | Target |
|-----------|---------|--------|
| PansyExporter | ~30% | 90% |
| BackgroundPansyExporter | ~40% | 90% |
| Label Management | 0% | 80% |
| CDL Processing | 0% | 80% |
| Debugger Core | 0% | 70% |

#### Priority Test Areas
- [ ] Complete PansyExporter test coverage
- [ ] Complete BackgroundPansyExporter test coverage
- [ ] LabelManager tests
- [ ] DebugApi wrapper tests
- [ ] Configuration persistence tests
- [ ] File I/O tests

### Phase 5: Lua Runtime Update (Priority: MEDIUM)

**Objective:** Update embedded Lua to latest version

#### Tasks
- [ ] Audit current Lua integration
- [ ] Identify Lua version (likely 5.4.x)
- [ ] Update to Lua 5.4.7+
- [ ] Test all Lua scripts in Debugger/Utilities/LuaScripts/
- [ ] Validate Lua documentation

### Phase 6: Code Modernization (Priority: MEDIUM)

**Objective:** Apply modern C# patterns and practices

#### Language Features
- [ ] Enable nullable reference types project-wide
- [ ] Use file-scoped namespaces consistently
- [ ] Use pattern matching where appropriate
- [ ] Use collection expressions
- [ ] Use primary constructors where applicable
- [ ] Use raw string literals for multi-line strings

#### Code Quality
- [ ] Enable additional analyzers
- [ ] Fix all analyzer warnings
- [ ] Consistent code style (editorconfig)
- [ ] Remove dead code
- [ ] Improve exception handling

### Phase 7: Documentation & CI/CD (Priority: LOW)

**Objective:** Improve development experience

#### Documentation
- [ ] Update README with .NET 10 requirements
- [ ] Update COMPILING.md
- [ ] API documentation for Pansy export
- [ ] Architecture documentation

#### CI/CD
- [ ] GitHub Actions for automated builds
- [ ] Automated test runs
- [ ] Code coverage reports
- [ ] Release automation

## 📅 Timeline

| Phase | Estimated Effort | Priority |
|-------|------------------|----------|
| Phase 1: .NET 10 | 1-2 days | HIGH |
| Phase 2: Avalonia | 1-2 days | HIGH |
| Phase 3: Built-in Libraries | 1 day | MEDIUM |
| Phase 4: Testing | 2-3 days | HIGH |
| Phase 5: Lua Update | 1 day | MEDIUM |
| Phase 6: Code Modernization | 2-3 days | MEDIUM |
| Phase 7: Documentation | 1 day | LOW |

**Total Estimated:** 9-13 days

## ⚠️ Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| .NET 10 breaking changes | HIGH | Test thoroughly, maintain net8.0 fallback |
| Avalonia API changes | MEDIUM | Review release notes, fix incrementally |
| Native code compatibility | HIGH | Test on all platforms |
| Performance regression | MEDIUM | Benchmark before/after |

## 🔄 Merge Strategy

1. Complete all phases on `modernization` branch
2. Full testing on all supported platforms
3. Merge `modernization` → `pansy-export`
4. Continue Pansy feature development
5. Eventually merge `pansy-export` → `master`

## 📝 Notes

- Baseline commit tagged as `v2.0.0-pansy-phase3`
- All work stays on `modernization` branch until complete
- Regular commits with descriptive messages
- Test after each major change

---

*Last Updated: January 26, 2026*
