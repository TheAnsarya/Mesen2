# Performance Improvement Plan — Emulation Speed & Audio

**Created:** 2026-02-26
**Epic Issue:** #418
**Branch:** `features-atari-lynx`

## Problem Statement

User reports while playing Dragon Warrior 4 (NES):
1. Audio stuttering (periodic glitches)
2. Periodic slowdowns (frame drops)
3. Cannot sustain 2x/3x fast-forward speed

## Root Cause Analysis Summary

### Finding 1 (CRITICAL — Nexen-specific): BackgroundCdlRecording activates full debugger
- `BackgroundCdlRecording = true` by default → `BackgroundPansyExporter` calls `InitializeDebugger()` on every ROM load
- Full C++ Debugger instantiated (all subsystems: breakpoints, trace logger, disassembler, CDL, memory counters)
- **Every CPU instruction** goes through `ProcessInstruction()` — address resolution, CDL marking, call stack, breakpoint checks
- **Every memory read/write** goes through `ProcessMemoryRead()`/`ProcessMemoryWrite()` — address resolution, CDL marking, access counters, breakpoint evaluation
- **Estimated overhead: 30-50% CPU slowdown**
- **Status: FIXED** — Default changed to `false`, config upgrade now sets `false` instead of `true`

### Finding 2 (HIGH — Mesen2): Audio buffer overflow at turbo speed
- `SoundMixer::PlayAudioBuffer` has no handling for speed > 100%
- Pitch adjustment only handles speed < 100% (slow-motion)
- At 2x/3x, audio buffer fills 2-3x faster than hardware drains, causing overflow → stutter
- `SoundResampler` disables rate feedback when speed != 100%
- Buffer health monitoring disabled when speed > 100%

### Finding 3 (HIGH — Mesen2): VideoDecoder spin-wait caps emulation speed
- `VideoDecoder::UpdateFrame()` busy-spins `while (_frameChanged) {}` with NO sleep/yield
- If video decode thread is slow, emulation thread burns CPU waiting
- At turbo speed, this creates a hard speed cap equal to video decode throughput
- Video filters (NTSC, scale2x) make decode slower

### Finding 4 (MEDIUM — Both): Synchronous save state operations
- All save operations (auto-save, recent-play, quick-save) block emulation thread
- Pipeline: AcquireLock → framebuffer zlib compression → state serialization → disk I/O → ReleaseLock
- Each save can take 5-15ms (NES frame = 16.7ms)
- **Partial fix applied:** SaveVideoData compression level reduced from 6 to 1

### Finding 5 (MEDIUM — Mesen2): Rewind buffer overhead at turbo
- Records full savestate every 30 frames (always active if buffer > 0)
- At 3x speed, fires 3x more often in real-time

### Finding 6 (LOW — Mesen2): Audio effects chain runs at turbo speed
- Full effects pipeline (resampling, EQ, reverb, crossfeed) runs for every audio frame
- At 3x speed, this is 3x the effects processing per real-time second

## Immediate Fixes Applied (This Session)

### Fix 1: BackgroundCdlRecording default → false (#419)
- `IntegrationConfig.cs`: Changed default from `true` to `false`
- `Configuration.cs`: Config upgrade now sets `false` (was `true`)
- Impact: Eliminates 30-50% overhead for all users who haven't manually enabled it

### Fix 2: SaveVideoData compression level 6 → 1 (#424)
- `SaveStateManager.cpp`: `MZ_DEFAULT_LEVEL` → `MZ_BEST_SPEED`
- Impact: 2-4x faster framebuffer compression on every save state

## Future Work (Issues Created)

### Phase 1: Quick Wins (Low Risk)
| Issue | Description | Estimated Impact |
|-------|-------------|-----------------|
| #419 | ✅ BackgroundCdlRecording default to false | 30-50% speed recovery |
| #424 | ✅ SaveVideoData compression level | 2-4x faster saves |

### Phase 2: Audio & Video (Medium Risk)
| Issue | Description | Estimated Impact |
|-------|-------------|-----------------|
| #420 | Audio frame skipping at turbo speed | Removes audio as speed bottleneck |
| #421 | VideoDecoder frame skipping/condition variable | Removes video as speed cap |

### Phase 3: Async Architecture (High Complexity)
| Issue | Description | Estimated Impact |
|-------|-------------|-----------------|
| #422 | Background save state thread | Eliminates save stalls |
| #423 | Reduce rewind frequency at turbo | ~3% CPU savings at 3x |

### Phase 4: Lightweight CDL (Future)
Create a CDL-only recording mode that doesn't need the full debugger:
- Track code/data boundaries only
- No breakpoints, trace logging, disassembly cache
- Use a simple bitfield per ROM byte (code/data/unknown)
- ~0.1% overhead instead of 30-50%

## What's NOT Causing Issues

These were investigated and found to be negligible:
- **Pansy export** — Only runs on timer (5 min) or user action, not per-frame. No C++ hooks.
- **Movie recording** — Just copies a few bytes of controller state per frame
- **Cheat checking** — Empty loop when no cheats active
- **Auto-save countdown** — Just a decrement per frame (the actual save is rare)
- **Frame limiter** — Correctly calculates delay for turbo speeds
- **Run-ahead** — Correctly disabled at speed > 100%

## Files Changed

| File | Change |
|------|--------|
| `UI/Config/IntegrationConfig.cs` | `BackgroundCdlRecording` default: `true` → `false` |
| `UI/Config/Configuration.cs` | Config upgrade: `true` → `false` + comment |
| `Core/Shared/SaveStateManager.cpp` | `MZ_DEFAULT_LEVEL` → `MZ_BEST_SPEED` |
