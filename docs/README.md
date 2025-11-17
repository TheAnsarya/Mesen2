# Mesen2 Project Documentation

**Project:** Mesen Multi-System Emulator  
**Version:** 2.1.1  
**Repository:** https://github.com/TheAnsarya/Mesen2  
**License:** GPL v3  
**Documentation Generated:** 2025

---

## Overview

Mesen2 is a multi-system emulator supporting:
- Nintendo Entertainment System (NES)
- Super Nintendo Entertainment System (SNES)
- Game Boy / Game Boy Color (GB/GBC)
- Game Boy Advance (GBA)
- PC Engine / TurboGrafx-16 (PCE)
- Sega Master System / Game Gear (SMS/GG)
- WonderSwan / WonderSwan Color (WS/WSC)

The project is built using a hybrid architecture:
- **Core Emulation:** C++17 (high-performance emulation cores)
- **User Interface:** C# .NET 8 with Avalonia UI (cross-platform)
- **Debugger:** Integrated debugging tools with assembly viewers, memory editors, and scripting

---

## Project Structure

```
Mesen2/
??? Core/                  # C++17 emulation cores
?   ??? Debugger/         # Core debugging functionality
?   ??? NES/              # NES emulation
?   ??? SNES/             # SNES emulation
?   ??? Gameboy/          # GB/GBC emulation
?   ??? GBA/              # GBA emulation
?   ??? PCE/              # PC Engine emulation
?   ??? SMS/              # Master System/Game Gear emulation
?   ??? WS/               # WonderSwan emulation
?   ??? Shared/           # Shared core utilities
?   ??? Netplay/          # Network play functionality
??? UI/                    # .NET 8 / Avalonia UI
?   ??? Debugger/         # Debugger UI components
?   ??? Windows/          # Window definitions
?   ??? Views/            # MVVM views
?   ??? ViewModels/       # MVVM view models
?   ??? Controls/         # Custom controls
?   ??? Config/           # Configuration management
?   ??? Interop/          # C++/C# interop layer
?   ??? Utilities/        # UI utilities
??? InteropDLL/           # C++/C# interop wrapper
??? Lua/                  # Lua scripting support
??? SevenZip/             # Archive support
??? Sdl/                  # SDL2 integration
??? Utilities/            # Build utilities
??? docs/                 # Project documentation (this folder)
??? ~docs/                # Session and analysis logs
```

---

## Technology Stack

### Core (C++)
- **Language:** C++17
- **Platform:** Cross-platform (Windows, Linux, macOS)
- **Key Libraries:**
  - SDL2 for input/audio/video
  - Custom emulation cores per system

### UI (C#)
- **Framework:** .NET 8.0
- **UI Library:** Avalonia 11.3 (cross-platform XAML)
- **Architecture:** MVVM (Model-View-ViewModel)
- **Key Dependencies:**
  - Avalonia.Desktop
  - Avalonia.AvaloniaEdit (code editor)
  - Avalonia.ReactiveUI
  - Dock.Avalonia (docking windows)
  - ELFSharp (ELF file parsing)

### Interop Layer
- P/Invoke for C# to C++ communication
- Native DLL/SO/Dylib exports from Core
- Marshaling for state objects and callbacks

---

## Key Features

### Emulation
- Cycle-accurate emulation for multiple systems
- Save states and rewind functionality
- Movie recording and playback
- Netplay support
- Cheat code support

### Debugging
- Multi-system debugger
- Disassembly viewer with syntax highlighting
- Memory viewer and hex editor
- Breakpoint management
- Watch expressions
- Trace logger
- Performance profiler
- Event viewer
- Graphics viewers:
  - Sprite viewer
  - Tile viewer
  - Tilemap viewer
  - Palette viewer
  - Tile editor
- Label and symbol import/export
- Script window (Lua scripting)
- Assembler integration

### Import/Export Formats
- **Debug Symbols:**
  - .dbg files (Mesen format)
  - .fns files (NES assembler)
  - ELF files
  - RGBDS symbol files
  - WLA-DX symbol files
  - Bass label files
  - PCEAS symbol files
  - SDCC symbol files
  - Legacy PCEAS format
- **Label Files:**
  - Mesen .mlb format
- **Save States:**
  - Mesen .mst format
- **Movies:**
  - Mesen movie format
- **ROM Formats:**
  - Multiple formats per system
  - Archive support (zip, 7z)

---

## Architecture Highlights

### Interop Architecture
The project uses a clean separation between C++ emulation core and C# UI:

1. **Core (C++)** exposes API through InteropDLL
2. **InteropDLL** provides C-compatible exports
3. **UI/Interop** classes in C# wrap native calls
4. **NotificationListener** receives events from Core

### Debugger Architecture
The debugger is deeply integrated with both Core and UI:
- Core provides debugging API (breakpoints, memory access, step execution)
- UI provides rich visualization and editing tools
- Symbol management bridges source code to machine code
- Multiple import formats for third-party assembler integration

---

## Documentation Index

- [API Reference](./api/README.md) - Core and UI API documentation
- [Integration Guide](./integration/README.md) - Third-party integration guidelines
- [Debugger Guide](./debugger/README.md) - Debugger feature documentation
- [DiztinGUIsh Integration](./diztinguish/README.md) - DiztinGUIsh integration planning ⭐
  - [Streaming Integration API](./diztinguish/streaming-integration.md) - Socket-based live tracing specification
  - [GitHub Issue Creation Guide](./diztinguish/GITHUB_ISSUE_CREATION.md) - How to create integration issues
  - [Implementation Tasks](./diztinguish/TODO.md) - Task breakdown and phases
- [Session Logs](../~docs/session_logs/) - Development session logs
- [Chat Logs](../~docs/chat_logs/) - Development chat logs

---

## 🔌 DiztinGUIsh Integration (NEW)

### Overview

Mesen2 is being integrated with [DiztinGUIsh](https://github.com/IsoFrieze/DiztinGUIsh) to enable **real-time live tracing** for SNES debugging and disassembly.

**Architecture:** Socket-based streaming (TCP)  
**Default Port:** 9998  
**Protocol:** Binary message protocol  
**Server:** Mesen2 (emulator)  
**Client:** DiztinGUIsh (disassembler)  

### Quick Links

- **Specification:** [streaming-integration.md](./diztinguish/streaming-integration.md) - Complete technical spec
- **Tasks:** [TODO.md](./diztinguish/TODO.md) - Implementation phases
- **Issues:** [github-issues.md](./diztinguish/github-issues.md) - GitHub issue templates
- **Project Board:** https://github.com/users/TheAnsarya/projects/4

### Socket Streaming API

**Connection Flow:**
```
Client (DiztinGUIsh) → Server (Mesen2) on port 9998
1. HANDSHAKE - Initial connection with version/ROM validation
2. HANDSHAKE_ACK - Server confirms connection
3. CONFIG_STREAM - Client configures streaming parameters
4. EXEC_TRACE - Server begins streaming execution traces (continuous)
5. Bidirectional communication for breakpoints, labels, memory access
```

**Message Types:**

| Type | Name | Direction | Purpose |
|------|------|-----------|---------|
| 0x01 | HANDSHAKE | C→S | Initial connection |
| 0x02 | HANDSHAKE_ACK | S→C | Connection confirmed |
| 0x03 | CONFIG_STREAM | C→S | Configure streaming |
| 0x10 | EXEC_TRACE | S→C | Execution traces (batched) |
| 0x11 | MEMORY_ACCESS | S→C | Memory read/write events |
| 0x12 | CPU_STATE | S→C | Full CPU snapshot |
| 0x13 | CDL_UPDATE | S→C | Code/Data Logger updates |
| 0x20 | BREAKPOINT_HIT | S→C | Breakpoint triggered |
| 0x21 | SET_BREAKPOINT | C→S | Create/modify breakpoint |
| 0x22 | LABEL_UPDATED | S→C | Label changed |
| 0x23 | PUSH_LABELS | C→S | Bulk label sync |
| 0x30 | REQUEST_MEMORY | C→S | Request memory dump |
| 0x31 | MEMORY_RESPONSE | S→C | Memory dump data |
| 0x40 | HEARTBEAT | Both | Keepalive |
| 0xFF | DISCONNECT | Both | Clean shutdown |

**Example: Execution Trace Message (0x10)**
```cpp
struct ExecTraceMessage {
    uint32_t pc;              // Program counter
    uint8_t opcode;           // Instruction opcode
    uint8_t m_flag;           // Memory size flag (8/16-bit)
    uint8_t x_flag;           // Index size flag (8/16-bit)
    uint8_t db_register;      // Data bank
    uint16_t dp_register;     // Direct page
    uint32_t effective_addr;  // Effective address
};
// 15 bytes per trace
```

**Performance:**
- Bandwidth: < 200 KB/s (15 Hz updates, batched)
- Emulation overhead: < 5%
- Latency: < 100ms (local connections)

### Implementation Status

**Phase 1: Foundation** (4 weeks) - 📋 Planning Complete
- [ ] S1: DiztinGUIsh Bridge server infrastructure
- [ ] S2: Execution trace streaming
- [ ] S3: Memory access and CDL integration

**Phase 2: Synchronization** (4 weeks) - ⏳ Not Started
- [ ] S4: CPU state snapshots
- [ ] S5: Label synchronization
- [ ] S6: Breakpoint bidirectional control

**Phase 3: Polish** (4 weeks) - ⏳ Not Started
- [ ] S7: UI integration
- [ ] S8: Error handling and reconnection

**Phase 4: Optimization** (4 weeks) - ⏳ Not Started
- [ ] S9: Performance tuning
- [ ] S10: Comprehensive testing

### Code Locations

**Implementation:**
- `Core/Debugger/DiztinguishBridge.h` - Bridge class (to be created)
- `Core/Debugger/DiztinguishBridge.cpp` - Implementation (to be created)
- `Utilities/Socket.h` - Socket wrapper (reused from netplay)
- `Utilities/Socket.cpp` - Socket implementation (existing)

**Reference:**
- `Core/Netplay/` - Existing socket usage patterns
- `Core/SNES/Debugger/` - SNES debugger integration points
- `Core/Debugger/CodeDataLogger.h` - CDL system

### See Also

- [streaming-integration.md](./diztinguish/streaming-integration.md) - Complete technical specification
- [DiztinGUIsh Repository](https://github.com/IsoFrieze/DiztinGUIsh) - SNES disassembler project

---

## Building

See [COMPILING.md](../COMPILING.md) in the root directory.

---

## Contributing

This is a fork of the official Mesen2 repository. For contributing to the upstream project, see:
https://github.com/SourMesen/Mesen2

---

## License

Mesen is available under the GPL V3 license.  
Copyright (C) 2014-2025 Sour

Full license text: http://www.gnu.org/licenses/gpl-3.0.en.html
