# Nexen

<p align="center">
  <img src="~assets/Nexen%20icon.png" alt="Nexen" width="128"/>
</p>

<p align="center">
  <strong>A powerful multi-system emulator for Windows, Linux, and macOS</strong>
</p>

<p align="center">
  <a href="https://github.com/TheAnsarya/Nexen/releases">
    <img src="https://img.shields.io/github/v/release/TheAnsarya/Nexen?include_prereleases" alt="Latest Release"/>
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/license-Unlicense%20%2B%20GPL--3.0-blue.svg" alt="License"/>
  </a>
</p>

---

## 📥 Downloads

Download pre-built binaries from the [Releases page](https://github.com/TheAnsarya/Nexen/releases/latest).

| Platform | Architecture | Download | Notes |
|----------|--------------|----------|-------|
| **Windows** | x64 | [Nexen-Windows-x64.exe](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-Windows-x64.exe) | Single-file |
| **Windows** | x64 (AoT) | [Nexen-Windows-x64-AoT.exe](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-Windows-x64-AoT.exe) | Native compiled |
| **Linux** | x64 | [Nexen-Linux-x64.AppImage](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-Linux-x64.AppImage) | AppImage (recommended) |
| **Linux** | ARM64 | [Nexen-Linux-ARM64.AppImage](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-Linux-ARM64.AppImage) | AppImage (recommended) |
| **macOS** | Intel (x64) | [Nexen-macOS-x64.zip](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-macOS-x64.zip) | Signed app bundle |
| **macOS** | Apple Silicon | [Nexen-macOS-ARM64.zip](https://github.com/TheAnsarya/Nexen/releases/latest/download/Nexen-macOS-ARM64.zip) | Signed app bundle |

> ℹ️ **Requirements:** Linux requires SDL2. macOS users: right-click → Open on first launch to bypass Gatekeeper.

---

## 🎮 Supported Systems

| System | CPU | Status |
| -------- | ----- | -------- |
| NES / Famicom | 6502 | ✅ Full |
| SNES / Super Famicom | 65816 | ✅ Full |
| Game Boy / Game Boy Color | LR35902 | ✅ Full |
| Game Boy Advance | ARM7TDMI | ✅ Full |
| Sega Master System / Game Gear | Z80 | ✅ Full |
| PC Engine / TurboGrafx-16 | HuC6280 | ✅ Full |
| WonderSwan / WonderSwan Color | V30MZ | ✅ Full |

## ✨ Key Features

### 🕹️ Emulation

- High-accuracy emulation across all supported systems
- Cycle-accurate CPU and PPU emulation
- Full coprocessor support (Super FX, SA-1, DSP, etc.)
- Comprehensive mapper/chip support
- Netplay multiplayer support

### 💾 Save States

- **Infinite Save States** - Unlimited timestamped saves per game
- **Visual Picker** - Grid view with screenshots and timestamps (Shift+F1)
- **Quick Save** - F1 to save, Shift+F1 to browse
- **Designated Slot** - F4 to load, Shift+F4 to save
- **Auto-Save** - Periodic quick saves and recent play saves
- **Per-Game Organization** - Saves organized by ROM

### 🎮 TAS Editor

- **Piano Roll View** - Visual timeline for frame-by-frame editing
- **Greenzone System** - Instant seeking with automatic savestates
- **Input Recording** - Capture and edit gameplay
- **Branches** - Compare alternate strategies
- **Multi-Format Import/Export** - BK2, FM2, SMV, LSMV, VBM, GMV, NMV
- **Lua Scripting** - Automate TAS workflows

### 🔧 Debugging

- **Disassembler** - Full CPU disassembly with labels
- **Memory Viewer** - Hex editor with search
- **Breakpoints** - Execution, read, and write breakpoints
- **Trace Logger** - CPU execution logging
- **Code/Data Logger (CDL)** - Track ROM usage
- **🌼 Pansy Export** - Export metadata to universal format

### 🎨 Video

- Multiple shader/filter options
- Integer scaling and aspect ratio correction
- NTSC and PAL filters
- HUD overlays for debugging

### 🔊 Audio

- High-quality audio resampling
- Per-channel volume control
- Audio recording (WAV)

##  Quick Start

### Windows

1. Download and extract the ZIP file
2. Run `Nexen.exe`
3. **File → Open** to load a ROM

### Linux

1. Download the AppImage
2. Make it executable: `chmod +x Nexen.AppImage`
3. Run: `./Nexen.AppImage`

### macOS

1. Download and extract the ZIP file
2. Move `Nexen.app` to Applications
3. Right-click → Open (first time, to bypass Gatekeeper)

## 📖 Documentation

| Document | Description |
| ---------- | ------------- |
| [Compiling](COMPILING.md) | Build from source |
| [TAS Editor Manual](docs/TAS-Editor-Manual.md) | Complete TAS workflow guide |
| [TAS Developer Guide](docs/TAS-Developer-Guide.md) | TAS system architecture |
| [Movie Format](docs/NEXEN_MOVIE_FORMAT.md) | NMV file format specification |
| [Release Guide](docs/RELEASE.md) | Creating releases |
| [API Documentation](docs/README.md) | Generated API docs |

### Developer Documentation

| Document | Description |
| ---------- | ------------- |
| [Architecture Overview](~docs/ARCHITECTURE-OVERVIEW.md) | System design |
| [C++ Development Guide](~docs/CPP-DEVELOPMENT-GUIDE.md) | Coding standards |
| [Pansy Integration](~docs/pansy-export-index.md) | Metadata export system |
| [Profiling Guide](~docs/PROFILING-GUIDE.md) | Performance profiling |

## ⌨️ Keyboard Shortcuts

### General

| Shortcut | Action |
| ---------- | -------- |
| F1 | Quick Save (Timestamped) |
| Shift+F1 | Browse Save States |
| F4 | Load Designated Slot |
| Shift+F4 | Save Designated Slot |
| Ctrl+S | Quick Save (Alt) |
| Ctrl+Shift+S | Save State to File |
| Ctrl+L | Load State from File |
| Escape | Pause/Resume |
| F11 | Toggle Fullscreen |

### TAS Editor

| Shortcut | Action |
| ---------- | -------- |
| Space | Play/Pause |
| F | Frame Advance |
| R | Frame Rewind |
| Ctrl+R | Toggle Recording |
| Insert | Insert Frame |
| Delete | Delete Frame |
| Ctrl+G | Go to Frame |
| Ctrl+B | Create Branch |

## 🔗 Related Projects

| Project | Description |
| --------- | ------------- |
| [🌼 Pansy](https://github.com/TheAnsarya/pansy) | Universal disassembly metadata format |
| [🌺 Peony](https://github.com/TheAnsarya/peony) | Multi-system disassembler |
| [🌸 Poppy](https://github.com/TheAnsarya/poppy) | Multi-system assembler |

## 🏗️ Building from Source

### Requirements

| Component | Version |
| ----------- | --------- |
| C++ | C++23 |
| .NET SDK | 10.0+ |
| SDL2 | Latest (Linux/macOS) |

See [COMPILING.md](COMPILING.md) for detailed instructions.

## 📜 License

Nexen is dual-licensed:

- **Nexen Additions (Unlicense):** All new features, modifications, and additions created for this fork are released into the public domain under [The Unlicense](https://unlicense.org).
- **Base Code (GPL v3):** The original [Mesen](https://github.com/SourMesen/Mesen2) code by Sour remains under the GNU General Public License v3.

See [LICENSE](LICENSE) for full details.

## 🙏 Acknowledgments

Nexen builds upon the excellent work of:

- **[Mesen](https://github.com/SourMesen/Mesen2)** by Sour - The original emulator codebase
- The emulation community for documentation and research
- Contributors and testers

---

<p align="center">
  Made with ❤️ for the retro gaming community
</p>
