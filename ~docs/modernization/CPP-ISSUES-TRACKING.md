# 🎫 Mesen2 C++ Modernization Issues

> **Document Created:** January 28, 2026
> **Last Updated:** January 28, 2026
> **Status:** � **IN PROGRESS**

This document tracks the GitHub issues and epics for the C++ core modernization project.

**GitHub Issues:** https://github.com/TheAnsarya/Mesen2/issues?q=label%3Acpp

## 📊 Status Overview

| Epic | GitHub | Status | Issues | Priority |
|------|--------|--------|--------|----------|
| Epic 8: Build System | [#40](https://github.com/TheAnsarya/Mesen2/issues/40) | 🔄 In Progress | 6 | HIGH |
| Epic 9: Testing | [#41](https://github.com/TheAnsarya/Mesen2/issues/41) | 📋 Planned | 8 | HIGH |
| Epic 10: Memory Safety | [#42](https://github.com/TheAnsarya/Mesen2/issues/42) | 📋 Planned | 6 | MEDIUM |
| Epic 11: Standard Library | [#43](https://github.com/TheAnsarya/Mesen2/issues/43) | 📋 Planned | 3 | MEDIUM |
| Epic 12: Performance | [#44](https://github.com/TheAnsarya/Mesen2/issues/44) | 📋 Planned | 2 | LOW |
| Epic 13: Documentation | [#45](https://github.com/TheAnsarya/Mesen2/issues/45) | 📋 Planned | 2 | LOW |

### 🚀 Recent Progress (January 29, 2026 - Session 6)

| Issue | Title | Status | Details |
|-------|-------|--------|---------|
| [#75](https://github.com/TheAnsarya/Mesen2/issues/75) | [[likely]]/[[unlikely]] Branch Hints | ✅ COMPLETE | 41 attributes in error paths |
| [#76](https://github.com/TheAnsarya/Mesen2/issues/76) | Expand constexpr Usage | 🔄 In Progress | 14 functions (ColorUtilities, Dsp, etc.) |
| [#77](https://github.com/TheAnsarya/Mesen2/issues/77) | Add [[nodiscard]] Attributes | 🔄 In Progress | ~179 attributes across all platforms |
| [#78](https://github.com/TheAnsarya/Mesen2/issues/78) | Adopt std::bit_cast | ✅ Research Done | LOW PRIORITY - minimal candidates |
| [#79](https://github.com/TheAnsarya/Mesen2/issues/79) | C++ Unit Testing Infrastructure | 📋 PLANNED | Epic 12 - Google Test integration |
| [#80](https://github.com/TheAnsarya/Mesen2/issues/80) | C++ Performance Benchmarking | 📋 PLANNED | Epic 13 - Benchmark framework |
| [#81](https://github.com/TheAnsarya/Mesen2/issues/81) | Code Documentation & Comments | 📋 PLANNED | Epic 14 - XML docs, Doxygen |
| [#82](https://github.com/TheAnsarya/Mesen2/issues/82) | Adopt std::span for buffers | ✅ COMPLETE | BatteryManager API, 26 files |

**Commits:**
- `2e9c5aaf`, `2b2e90a9` - [[unlikely]] implementation
- `e5232022` - [[nodiscard]] utility functions
- `9220b5d3` - constexpr expansion
- `8db3eeef`, `1672f627` - [[nodiscard]] Core/Shared getters
- `e3d73e45` - style: newlines at end of header files
- `6af709d1` - feat: std::span for BatteryManager API (#82)
- `6df6486e` - feat: [[nodiscard]] to WonderSwan getters
- `59738a1e` - docs: update cpp-modernization-opportunities with current status

---

## Epic 8: C++ Build System Modernization

**GitHub:** [#40](https://github.com/TheAnsarya/Mesen2/issues/40)
**Status:** 🔄 In Progress
**Priority:** HIGH
**Estimated Effort:** 1 week
**Depends On:** None

### Description
Modernize the C++ build system to use VS2026 tooling, enable C++23, and integrate modern analysis tools.

### Issues

#### Issue 8.1: Update Platform Toolset to v144 (VS2026)
**GitHub:** [#46](https://github.com/TheAnsarya/Mesen2/issues/46) ✅ CLOSED
**Priority:** HIGH
**Labels:** `build`, `modernization`, `cpp`

**Description:**
Update all C++ project files to use the VS2026 platform toolset (v144).

**Tasks:**
- [x] Update Core.vcxproj
- [x] Update Utilities.vcxproj
- [x] Update Windows.vcxproj
- [x] Update InteropDLL.vcxproj
- [x] Update Lua.vcxproj
- [x] Update SevenZip.vcxproj
- [x] Update PGOHelper.vcxproj
- [x] Verify builds on Windows

**Acceptance Criteria:**
- All projects use PlatformToolset v144 ✅
- All projects build without errors ✅

---

#### Issue 8.2: Enable C++23 Language Standard
**GitHub:** [#47](https://github.com/TheAnsarya/Mesen2/issues/47) ✅ CLOSED
**Priority:** HIGH
**Labels:** `build`, `modernization`, `cpp`

**Description:**
Enable C++23 language standard across all C++ projects.

**Tasks:**
- [ ] Add `<LanguageStandard>stdcpp23</LanguageStandard>` to all vcxproj files
- [ ] Update makefile for Linux builds
- [ ] Fix any C++23 compatibility issues
- [ ] Document any deferred C++26 features

**Acceptance Criteria:**
- All projects compile with C++23
- No new warnings from standard change

---

#### Issue 8.3: Enable Maximum Warning Level
**Priority:** MEDIUM
**Labels:** `build`, `code-quality`, `cpp`

**Description:**
Enable warning level 4 (/W4) and optionally treat warnings as errors.

**Tasks:**
- [ ] Enable /W4 warning level
- [ ] Document and fix all warnings
- [ ] Consider /WX (warnings as errors) for CI

**Acceptance Criteria:**
- Clean build at /W4
- All warnings documented or fixed

---

#### Issue 8.4: Integrate Clang-Tidy
**Priority:** MEDIUM
**Labels:** `tooling`, `code-quality`, `cpp`

**Description:**
Set up clang-tidy for static analysis and automated code modernization suggestions.

**Tasks:**
- [ ] Create .clang-tidy configuration file
- [ ] Configure recommended checks
- [ ] Document usage instructions
- [ ] Add to CI pipeline (optional)

**Acceptance Criteria:**
- .clang-tidy file with appropriate checks
- No critical issues reported

---

#### Issue 8.5: Configure AddressSanitizer for Debug Builds
**Priority:** LOW
**Labels:** `tooling`, `testing`, `cpp`

**Description:**
Enable AddressSanitizer (ASan) for debug builds to catch memory errors.

**Tasks:**
- [ ] Create debug configuration with ASan
- [ ] Document how to run with ASan
- [ ] Fix any issues found

**Acceptance Criteria:**
- Debug build runs cleanly with ASan
- No memory errors detected

---

#### Issue 8.6: Update Linux Build Configuration
**Priority:** MEDIUM
**Labels:** `build`, `linux`, `cpp`

**Description:**
Update makefile and CMake (if applicable) for Linux builds with C++23.

**Tasks:**
- [ ] Update makefile with -std=c++23
- [ ] Enable -Wall -Wextra -Wpedantic
- [ ] Test build on Linux
- [ ] Update build documentation

**Acceptance Criteria:**
- Linux build succeeds with C++23
- Clean build with all warnings

---

## Epic 9: C++ Testing Infrastructure

**Status:** 📋 Planned
**Priority:** HIGH
**Estimated Effort:** 1-2 weeks
**Depends On:** Epic 8

### Description
Establish comprehensive unit testing infrastructure for C++ core components using Google Test or Catch2.

### Issues

#### Issue 9.1: Integrate Testing Framework
**Priority:** HIGH
**Labels:** `testing`, `infrastructure`, `cpp`

**Description:**
Add Google Test (gtest) or Catch2 testing framework to the solution.

**Tasks:**
- [ ] Evaluate gtest vs Catch2
- [ ] Add chosen framework via vcpkg or submodule
- [ ] Create test project (Mesen.Core.Tests)
- [ ] Configure test discovery in VS

**Acceptance Criteria:**
- Test project builds
- Sample test runs successfully

---

#### Issue 9.2: Create NES CPU Instruction Tests
**Priority:** HIGH
**Labels:** `testing`, `nes`, `cpu`, `cpp`

**Description:**
Create comprehensive tests for all 6502 CPU instructions.

**Tasks:**
- [ ] Test all official opcodes
- [ ] Test unofficial/undocumented opcodes
- [ ] Test flags behavior
- [ ] Test addressing modes
- [ ] Compare with nestest.nes results

**Acceptance Criteria:**
- All 256 opcodes tested
- Flags verified for each instruction

---

#### Issue 9.3: Create SNES CPU Instruction Tests
**Priority:** HIGH
**Labels:** `testing`, `snes`, `cpu`, `cpp`

**Description:**
Create comprehensive tests for 65816 CPU instructions.

**Tasks:**
- [ ] Test all official opcodes
- [ ] Test 8-bit vs 16-bit modes
- [ ] Test bank switching
- [ ] Test interrupt handling

**Acceptance Criteria:**
- All opcodes tested
- Mode switching verified

---

#### Issue 9.4: Create Game Boy CPU Tests
**Priority:** MEDIUM
**Labels:** `testing`, `gameboy`, `cpu`, `cpp`

**Description:**
Create tests for Sharp LR35902 CPU instructions.

**Tasks:**
- [ ] Test all opcodes
- [ ] Test CB-prefixed instructions
- [ ] Test interrupt handling
- [ ] Compare with known test ROMs

**Acceptance Criteria:**
- All opcodes tested
- Blargg's tests pass

---

#### Issue 9.5: Create Memory Mapping Tests
**Priority:** HIGH
**Labels:** `testing`, `memory`, `cpp`

**Description:**
Create tests for memory mapping across all platforms.

**Tasks:**
- [ ] Test NES memory mapping
- [ ] Test SNES bank switching
- [ ] Test Game Boy memory banking
- [ ] Test mirroring behavior

**Acceptance Criteria:**
- All memory regions tested
- Bank switching verified

---

#### Issue 9.6: Create PPU Timing Tests
**Priority:** MEDIUM
**Labels:** `testing`, `ppu`, `cpp`

**Description:**
Create tests for PPU timing accuracy.

**Tasks:**
- [ ] Test scanline timing
- [ ] Test VBlank timing
- [ ] Test sprite evaluation timing
- [ ] Compare with known timing test ROMs

**Acceptance Criteria:**
- Timing tests pass
- Known test ROMs verified

---

#### Issue 9.7: Create Save State Tests
**Priority:** MEDIUM
**Labels:** `testing`, `savestate`, `cpp`

**Description:**
Create tests for save state integrity.

**Tasks:**
- [ ] Test save/restore cycle
- [ ] Test state across resets
- [ ] Test compression handling
- [ ] Verify no data loss

**Acceptance Criteria:**
- States save and restore correctly
- No corruption after multiple cycles

---

#### Issue 9.8: Configure CI Test Execution
**Priority:** LOW
**Labels:** `testing`, `ci`, `cpp`

**Description:**
Configure continuous integration to run C++ tests.

**Tasks:**
- [ ] Add test step to GitHub Actions
- [ ] Configure test result reporting
- [ ] Add test coverage reporting

**Acceptance Criteria:**
- Tests run on every PR
- Results visible in GitHub

---

## Epic 10: C++ Memory Safety Modernization

**Status:** 📋 Planned
**Priority:** MEDIUM
**Estimated Effort:** 2-3 weeks
**Depends On:** Epic 9

### Description
Replace raw pointers with smart pointers and modern containers for improved memory safety.

### Issues

#### Issue 10.1: Migrate Core/Shared to Smart Pointers
**Priority:** HIGH
**Labels:** `modernization`, `memory`, `cpp`

**Description:**
Replace raw pointer allocations in Core/Shared with smart pointers.

**Tasks:**
- [ ] Identify all raw new/delete in Core/Shared
- [ ] Replace with std::unique_ptr or std::shared_ptr
- [ ] Update ownership semantics documentation

**Acceptance Criteria:**
- No raw new/delete in Core/Shared
- Tests pass

---

#### Issue 10.2: Migrate NES Core to Smart Pointers
**Priority:** MEDIUM
**Labels:** `modernization`, `memory`, `nes`, `cpp`

**Description:**
Modernize memory management in NES emulation core.

**Tasks:**
- [ ] Audit NES memory allocations
- [ ] Replace with smart pointers
- [ ] Verify emulation accuracy

---

#### Issue 10.3: Migrate SNES Core to Smart Pointers
**Priority:** MEDIUM
**Labels:** `modernization`, `memory`, `snes`, `cpp`

**Description:**
Modernize memory management in SNES emulation core.

---

#### Issue 10.4: Migrate Game Boy Core to Smart Pointers
**Priority:** MEDIUM
**Labels:** `modernization`, `memory`, `gameboy`, `cpp`

---

#### Issue 10.5: Migrate GBA Core to Smart Pointers
**Priority:** LOW
**Labels:** `modernization`, `memory`, `gba`, `cpp`

---

#### Issue 10.6: Adopt std::span for Buffer Parameters
**GitHub:** [#82](https://github.com/TheAnsarya/Mesen2/issues/82) ✅ CLOSED
**Priority:** MEDIUM
**Labels:** `modernization`, `cpp`

**Description:**
Replace raw pointer + size pairs with std::span.

**Tasks:**
- [x] Identify functions with ptr+size patterns
- [x] Replace with std::span<T> or std::span<const T>
- [x] Update call sites

**Implementation (January 29, 2026):**
- BatteryManager::SaveBattery: `std::span<const uint8_t>`
- BatteryManager::LoadBattery: `std::span<uint8_t>`
- Updated 26 files across all platforms (NES, SNES, GB, GBA, SMS, PCE, WS)

**Acceptance Criteria:**
- Consistent use of std::span ✅
- No buffer overflows possible ✅

---

#### Issue 10.7: Adopt std::array for Fixed-Size Arrays
**Priority:** LOW
**Labels:** `modernization`, `cpp`

**Description:**
Replace C-style arrays with std::array where size is known at compile time.

---

#### Issue 10.8: Replace C-Style Casts
**Priority:** LOW
**Labels:** `modernization`, `code-quality`, `cpp`

**Description:**
Replace C-style casts with appropriate C++ casts.

**Tasks:**
- [ ] Replace with static_cast where appropriate
- [ ] Replace with reinterpret_cast for type punning
- [ ] Replace with const_cast where needed
- [ ] Use bit_cast for type reinterpretation

---

#### Issue 10.9: Add [[nodiscard]] Attributes
**Priority:** LOW
**Labels:** `modernization`, `code-quality`, `cpp`

**Description:**
Add [[nodiscard]] to functions where return value should not be ignored.

---

#### Issue 10.10: Expand constexpr Usage
**Priority:** LOW
**Labels:** `modernization`, `performance`, `cpp`

**Description:**
Mark functions and variables as constexpr where possible.

---

## Epic 11: C++ Standard Library Modernization

**Status:** 📋 Planned
**Priority:** MEDIUM
**Estimated Effort:** 2 weeks
**Depends On:** Epic 10

### Description
Leverage modern C++ standard library features for cleaner, safer code.

### Issues

#### Issue 11.1: Adopt std::ranges Algorithms
**Priority:** MEDIUM
**Labels:** `modernization`, `cpp`

**Description:**
Replace manual loops with std::ranges algorithms.

---

#### Issue 11.2: Migrate to std::format
**Priority:** MEDIUM
**Labels:** `modernization`, `cpp`

**Description:**
Replace sprintf/snprintf with std::format.

---

#### Issue 11.3: Adopt std::filesystem
**Priority:** LOW
**Labels:** `modernization`, `cpp`

**Description:**
Use std::filesystem for file operations where not already used.

---

#### Issue 11.4: Use std::optional for Optional Values
**Priority:** MEDIUM
**Labels:** `modernization`, `cpp`

**Description:**
Replace null pointers for optional values with std::optional.

---

#### Issue 11.5: Use std::variant for Type-Safe Unions
**Priority:** LOW
**Labels:** `modernization`, `cpp`

---

#### Issue 11.6: Apply Structured Bindings
**Priority:** LOW
**Labels:** `modernization`, `cpp`

---

#### Issue 11.7: Apply [[likely]]/[[unlikely]] Attributes
**Priority:** LOW
**Labels:** `performance`, `cpp`

**Description:**
Add branch prediction hints to hot paths.

---

#### Issue 11.8: Use std::string_view for Non-Owning Strings
**Priority:** LOW
**Labels:** `modernization`, `cpp`

---

## Epic 12: C++ Performance Optimization

**Status:** 📋 Planned
**Priority:** LOW
**Estimated Effort:** 2-3 weeks
**Depends On:** Epic 11

### Description
Profile and optimize critical emulation paths.

### Issues

#### Issue 12.1: Set Up Profiling Infrastructure
**Priority:** MEDIUM
**Labels:** `performance`, `tooling`, `cpp`

**Description:**
Configure profiling tools (VTune, Tracy, or perf).

---

#### Issue 12.2: Create Performance Benchmarks
**Priority:** MEDIUM
**Labels:** `performance`, `testing`, `cpp`

**Description:**
Create benchmarks for critical components.

---

#### Issue 12.3: Optimize CPU Hot Paths
**Priority:** MEDIUM
**Labels:** `performance`, `cpu`, `cpp`

---

#### Issue 12.4: Optimize Memory Access Patterns
**Priority:** LOW
**Labels:** `performance`, `memory`, `cpp`

---

#### Issue 12.5: Evaluate SIMD Opportunities
**Priority:** LOW
**Labels:** `performance`, `simd`, `cpp`

---

#### Issue 12.6: Document Performance Characteristics
**Priority:** LOW
**Labels:** `documentation`, `performance`, `cpp`

---

## Epic 13: C++ Documentation

**Status:** 📋 Planned
**Priority:** LOW
**Estimated Effort:** Ongoing
**Depends On:** All other epics

### Description
Comprehensive documentation for C++ codebase.

### Issues

#### Issue 13.1: Set Up Doxygen
**Priority:** LOW
**Labels:** `documentation`, `tooling`, `cpp`

---

#### Issue 13.2: Document Core/Shared APIs
**Priority:** LOW
**Labels:** `documentation`, `cpp`

---

#### Issue 13.3: Document Emulation Architecture
**Priority:** LOW
**Labels:** `documentation`, `cpp`

---

#### Issue 13.4: Document Emulation Accuracy Notes
**Priority:** LOW
**Labels:** `documentation`, `cpp`

---

#### Issue 13.5: Apply clang-format
**Priority:** LOW
**Labels:** `code-quality`, `cpp`

**Description:**
Create .clang-format and apply consistent formatting.

---

## 📋 Issue Creation Checklist

When creating issues in GitHub:

1. **Title Format:** `[Epic X.Y] Brief Description`
2. **Labels:** Apply all relevant labels
3. **Milestone:** Assign to C++ Modernization milestone
4. **Project:** Add to Mesen2 Modernization project board
5. **Parent Epic:** Link to parent epic issue
6. **Dependencies:** Note any blocking issues
