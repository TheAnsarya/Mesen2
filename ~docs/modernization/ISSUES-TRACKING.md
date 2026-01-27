# 🎫 Mesen2 Modernization Issues

This document tracks the GitHub issues and epics for the Mesen2 modernization project.

## 📋 Epics

### Epic 1: .NET 10 Migration
**Issue #:** (To be created)
**Priority:** HIGH
**Estimate:** 2 days

**Description:**
Migrate Mesen2 from .NET 8.0 to .NET 10.0 to leverage latest runtime features, performance improvements, and language capabilities.

**Acceptance Criteria:**
- All .csproj files target net10.0
- Project builds without errors on Windows, Linux, macOS
- All existing functionality works correctly
- Performance is equal or better than .NET 8

**Sub-tasks:**
1. Update UI.csproj TargetFramework
2. Update DataBox.csproj TargetFramework
3. Update Mesen.Tests.csproj TargetFramework
4. Update RuntimeIdentifier configurations
5. Fix any .NET 10 breaking changes
6. Test native interop (C++ core)
7. Cross-platform testing

---

### Epic 2: Avalonia Update
**Issue #:** (To be created)
**Priority:** HIGH
**Estimate:** 2 days

**Description:**
Update Avalonia and all related packages from 11.3.1 to 11.3.11+ for bug fixes, performance improvements, and new features.

**Acceptance Criteria:**
- All Avalonia packages updated to 11.3.11+
- All UI components render correctly
- No XAML errors or warnings
- All custom controls work properly

**Package Updates:**
- Avalonia 11.3.1 → 11.3.11
- Avalonia.Desktop 11.3.1 → 11.3.11
- Avalonia.Controls.ColorPicker 11.3.1 → 11.3.11
- Avalonia.Diagnostics 11.3.1 → 11.3.11
- Avalonia.ReactiveUI 11.3.1 → 11.3.11
- Avalonia.Themes.Fluent 11.3.1 → 11.3.11
- Avalonia.AvaloniaEdit 11.3.0 → Latest
- Dock.Avalonia 11.3.0.2 → Latest
- Dock.Model.Mvvm 11.3.0.2 → Latest

---

### Epic 3: Built-in Libraries Migration
**Issue #:** (To be created)
**Priority:** MEDIUM
**Estimate:** 1 day

**Description:**
Replace custom implementations with modern .NET built-in libraries for better maintainability and performance.

**Key Migrations:**
1. **CRC32:** Custom implementation → System.IO.Hashing.Crc32
2. **JSON:** Audit for source generator opportunities
3. **Collections:** Use collection expressions
4. **Compression:** Ensure using System.IO.Compression

**Acceptance Criteria:**
- All custom CRC32 code replaced
- PansyExporter uses System.IO.Hashing
- Identical output for all operations
- No performance regression

---

### Epic 4: Comprehensive Testing
**Issue #:** (To be created)
**Priority:** HIGH
**Estimate:** 3 days

**Description:**
Implement comprehensive unit and integration tests for critical components.

**Coverage Goals:**
- PansyExporter: 90%
- BackgroundPansyExporter: 90%
- LabelManager: 80%
- CDL Processing: 80%
- Configuration: 80%

**Infrastructure:**
- xUnit framework
- coverlet for code coverage
- CI integration

**Acceptance Criteria:**
- Test project runs with `dotnet test`
- Coverage reports generated
- All critical paths tested
- Tests pass on all platforms

---

### Epic 5: Lua Runtime Update
**Issue #:** (To be created)
**Priority:** MEDIUM
**Estimate:** 1 day

**Description:**
Update the embedded Lua runtime to the latest version (5.4.7+).

**Acceptance Criteria:**
- Lua updated to 5.4.7+
- All example scripts work
- Lua documentation updated
- No breaking changes for existing scripts

---

### Epic 6: Code Modernization
**Issue #:** (To be created)
**Priority:** MEDIUM
**Estimate:** 3 days

**Description:**
Apply modern C# 13/14 patterns and best practices across the codebase.

**Modernization Items:**
- Nullable reference types
- File-scoped namespaces
- Pattern matching
- Collection expressions
- Primary constructors
- Raw string literals
- Analyzers

**Acceptance Criteria:**
- No nullable warnings
- Consistent code style
- All analyzer warnings fixed
- Code passes editorconfig rules

---

### Epic 7: Documentation & CI/CD
**Issue #:** (To be created)
**Priority:** LOW
**Estimate:** 1 day

**Description:**
Update documentation and implement CI/CD automation.

**Documentation Updates:**
- README.md - .NET 10 requirements
- COMPILING.md - Updated build instructions
- Pansy export API documentation
- Architecture documentation

**CI/CD:**
- GitHub Actions workflow
- Automated builds
- Automated tests
- Code coverage reports

---

## 📊 Issue Template

```markdown
## Description
[Clear description of the task]

## Acceptance Criteria
- [ ] Criterion 1
- [ ] Criterion 2

## Technical Notes
[Any technical details or considerations]

## Related Issues
- #[number] - [description]

## Estimate
[Story points or time estimate]
```

---

## 🏷️ Labels

| Label | Description | Color |
|-------|-------------|-------|
| `modernization` | Part of modernization effort | Blue |
| `epic` | Epic/parent issue | Purple |
| `.net-10` | .NET 10 related | Green |
| `avalonia` | Avalonia UI related | Cyan |
| `testing` | Test related | Yellow |
| `high-priority` | High priority | Red |
| `medium-priority` | Medium priority | Orange |
| `low-priority` | Low priority | Gray |

---

## 📅 Sprint Planning

### Sprint 1: Foundation (Days 1-2)
- [ ] Epic 1: .NET 10 Migration
- [ ] Start Epic 2: Avalonia Update

### Sprint 2: Dependencies (Days 3-4)
- [ ] Complete Epic 2: Avalonia Update
- [ ] Epic 3: Built-in Libraries

### Sprint 3: Quality (Days 5-7)
- [ ] Epic 4: Comprehensive Testing
- [ ] Epic 5: Lua Update

### Sprint 4: Polish (Days 8-10)
- [ ] Epic 6: Code Modernization
- [ ] Epic 7: Documentation

---

*Last Updated: January 26, 2026*
