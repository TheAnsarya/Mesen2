# Debugging Guide

## Overview

Nexen includes integrated debugging tools for tracing execution, inspecting memory, and validating emulator behavior.

## Primary Tools

- Disassembler: inspect instructions and labels.
- Memory Viewer: inspect and edit memory values.
- Breakpoints: execution, read, and write stops.
- Trace Logger: export execution logs for analysis.
- Code/Data Logger: inspect ROM usage patterns.

## Typical Debug Session

1. Load ROM and open debugger windows.
2. Set breakpoints near target code.
3. Run until break.
4. Inspect registers, memory, and call flow.
5. Use trace logging for deeper sequence analysis.

## GUI Tips

- Keep memory and disassembly windows side-by-side.
- Use breakpoints and rewind together for fast iteration.
- Save states before risky edits or long trace sessions.

## Panel Walkthrough (Screenshot Anchors)

### Walkthrough A: Breakpoint and Disassembly Inspection

1. Open the Disassembler panel.
2. Add an execution breakpoint at target code.
3. Run until breakpoint is hit.
4. Inspect current instruction context and labels.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Disassembler Panel (Initial) | `debug-a-01-disassembler-initial` | `docs/screenshots/debugging/a-01-disassembler-initial.png` |
| 2 | Breakpoint Configuration | `debug-a-02-breakpoint-config` | `docs/screenshots/debugging/a-02-breakpoint-config.png` |
| 3 | Disassembler (Breakpoint Hit) | `debug-a-03-breakpoint-hit` | `docs/screenshots/debugging/a-03-breakpoint-hit.png` |

### Walkthrough B: Memory and Trace Correlation

1. Open Memory Viewer panel and navigate to target address range.
2. Open Trace Logger and start capture.
3. Execute scenario and stop trace capture.
4. Compare trace events with observed memory transitions.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Memory Viewer (Target Region) | `debug-b-01-memory-region` | `docs/screenshots/debugging/b-01-memory-region.png` |
| 2 | Trace Logger (Capture Active) | `debug-b-02-trace-active` | `docs/screenshots/debugging/b-02-trace-active.png` |
| 3 | Trace and Memory Side-by-Side | `debug-b-03-trace-memory-correlation` | `docs/screenshots/debugging/b-03-trace-memory-correlation.png` |

## Related Links

- [Debugger Performance](DEBUGGER-PERFORMANCE.md)
- [Movie System Guide](Movie-System.md)
- [Systems Documentation](systems/README.md)
