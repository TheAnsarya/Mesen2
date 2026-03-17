# Atari 2600 TIA Timing Spike and Smoke-Test Harness Plan

Issue linkage: [#698](https://github.com/TheAnsarya/Nexen/issues/698)

Parent issue: [#695](https://github.com/TheAnsarya/Nexen/issues/695)

## Goal

Define and implement a deterministic TIA timing spike with scriptable pass/fail outputs for a baseline smoke ROM set.

## Scope

- Scanline stepping model with explicit cycle accounting.
- Minimal visibility checkpoints for player, missile, and ball timing.
- Deterministic harness output format.
- Documented run commands and expected output signatures.

## Timing Checkpoint Matrix

| Checkpoint ID | Assertion | Output |
|---|---|---|
| TIA-CP-01 | Scanline counter increments exactly as expected across frame boundary | `PASS TIA-CP-01` |
| TIA-CP-02 | WSYNC hold/release behavior preserves cycle ordering | `PASS TIA-CP-02` |
| TIA-CP-03 | Object visibility checkpoint appears at expected cycle bucket | `PASS TIA-CP-03` |
| TIA-CP-04 | Repeat runs produce identical summary digest | `PASS TIA-CP-04` |

## Harness Output Contract

- Machine-readable line format:
- `CHECKPOINT <id> <PASS|FAIL> <context>`
- Final summary line:
- `HARNESS_SUMMARY PASS=<n> FAIL=<n> DIGEST=<hash>`
- Baseline ROM set line format:
- `ROM_RESULT <rom-name> <PASS|FAIL> PASS=<n> FAIL=<n> DIGEST=<hash>`
- Baseline ROM set summary line:
- `ROM_SET_SUMMARY PASS=<n> FAIL=<n> DIGEST=<hash>`

## Run Command Templates

```powershell
.\bin\win-x64\Release\Core.Tests.exe --gtest_filter=Atari2600TimingSpikeHarnessTests.TimingSpikeHarnessHasStableScanlineDeltas --gtest_brief=1
```

```powershell
.\bin\win-x64\Release\Core.Tests.exe --gtest_filter=Atari2600TimingSpikeHarnessTests.* --gtest_brief=1
```

```powershell
.\bin\win-x64\Release\Core.Tests.exe --gtest_filter=Atari2600TimingSpikeHarnessTests.BaselineRomSet* --gtest_brief=1
```

```powershell
.\bin\win-x64\Release\Core.Tests.exe --gtest_filter=GenesisM68kBoundaryScaffoldTests.*:Atari2600TimingSpikeHarnessTests.* --gtest_brief=1
```

## Expected Outcomes

- All baseline checkpoints return `PASS`.
- Digest remains stable across repeated runs on unchanged code.
- Failing checkpoint reports include cycle/scanline context.
- Baseline ROM set run emits one `ROM_RESULT` line per ROM and a final `ROM_SET_SUMMARY` line.
- Current focused run prints:
	- `[==========] 10 tests from 2 test suites ran.`
	- `TIMING_SPIKE SUMMARY STABLE=true DIGEST=<stable-hash>` (from harness output lines)

## Execution Evidence: Promoted TIA Follow-Through

Status: Completed from deferred backlog (2026-03-17)

### Issue [#721](https://github.com/TheAnsarya/Nexen/issues/721)

- Added deterministic WSYNC edge counting and HMOVE strobe/apply timing behavior.
- Added focused tests in `Atari2600TiaPhaseATests` for WSYNC/HMOVE timing progression.

### Issue [#722](https://github.com/TheAnsarya/Nexen/issues/722)

- Replaced placeholder debug gradient with deterministic register-driven playfield/object rendering scaffold.
- Added focused tests in `Atari2600RenderPhaseATests` for playfield changes and layer overlays.

### Issue [#723](https://github.com/TheAnsarya/Nexen/issues/723)

- Implemented deterministic two-channel TIA audio stepping and mixer accumulator state.
- Added AUDC/AUDF/AUDV register read/write behavior and audio metadata integration through console audio API.
- Added focused tests in `Atari2600AudioPhaseATests` for register semantics, deterministic output, and mixer reset behavior.

Validation command used for promoted TIA work:

```powershell
.\bin\win-x64\Release\Core.Tests.exe --gtest_filter=Atari2600AudioPhaseATests.*:Atari2600RenderPhaseATests.*:Atari2600TiaPhaseATests.*:Atari2600RiotPhaseATests.*:Atari2600CpuPhaseATests.*:Atari2600TimingSpikeHarnessTests.*:Atari2600MapperPhaseATests.*:Atari2600MapperPhaseBTests.*:Atari2600MapperPhaseCTests.* --gtest_brief=1
```

Result: 31 tests from 9 suites passed.

## Deferred Future-Work Linkage

Status: Future Work only. Do not start these issues until explicitly scheduled.

Completed from this deferred set:

- TIA timing follow-through: [#721](https://github.com/TheAnsarya/Nexen/issues/721)
- TIA render follow-through: [#722](https://github.com/TheAnsarya/Nexen/issues/722)
- TIA audio follow-through: [#723](https://github.com/TheAnsarya/Nexen/issues/723)

- Parent future-work epic: [#717](https://github.com/TheAnsarya/Nexen/issues/717)
- Compatibility harness expansion: [#725](https://github.com/TheAnsarya/Nexen/issues/725)

## Dependencies

- [Atari 2600 TIA Video and Timing](../research/platform-parity/atari-2600/tia-video-timing.md)
- [Atari 2600 Frame Execution Model](../research/platform-parity/atari-2600/frame-execution-model.md)
