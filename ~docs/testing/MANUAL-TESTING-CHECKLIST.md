# Nexen v1.0.0 Manual Testing Checklist

> **Purpose:** Comprehensive testing checklist for all Nexen features implemented since forking from Mesen2.
> **Date Created:** 2026-02-02
> **Target Version:** v1.0.0

---

## Table of Contents

1. [Test Environment Setup](#1-test-environment-setup)
2. [TAS Editor Testing](#2-tas-editor-testing)
3. [Save State System Testing](#3-save-state-system-testing)
4. [Movie Converter Testing](#4-movie-converter-testing)
5. [Pansy Export Testing](#5-pansy-export-testing)
6. [Keyboard Shortcuts Testing](#6-keyboard-shortcuts-testing)
7. [UI/UX Testing](#7-uiux-testing)
8. [Multi-System Testing](#8-multi-system-testing)
9. [Performance Testing](#9-performance-testing)
10. [Build & Platform Testing](#10-build--platform-testing)

---

## 1. Test Environment Setup

### 1.1 Required Test ROMs

Prepare test ROMs for each supported system:

- [ ] **NES:** Any licensed game (e.g., Super Mario Bros., Mega Man 2)
- [ ] **SNES:** Any licensed game (e.g., Super Mario World, Chrono Trigger)
- [ ] **Game Boy:** Any licensed game (e.g., Pokemon Red/Blue, Tetris)
- [ ] **Game Boy Color:** Any licensed GBC game (e.g., Pokemon Crystal)
- [ ] **Game Boy Advance:** Any licensed GBA game (e.g., Metroid Fusion)
- [ ] **Master System:** Any licensed game (e.g., Sonic the Hedgehog)
- [ ] **Game Gear:** Any licensed game
- [ ] **PC Engine:** Any licensed game (e.g., Bonk's Adventure)
- [ ] **WonderSwan:** Any licensed game

### 1.2 Test Environment Checklist

- [ ] Clean install of Nexen (delete all settings/saves first)
- [ ] Note system specs: OS version, CPU, RAM, GPU
- [ ] Enable debug logging if available
- [ ] Prepare screen recording software for bug reproduction

---

## 2. TAS Editor Testing

### 2.1 Basic TAS Editor Opening

- [ ] **Open TAS Editor** - Tools → TAS Editor (or keyboard shortcut)
- [ ] **Verify window opens** without crash
- [ ] **Verify menu bar** displays: File, Edit, Playback, Recording, View, Tools
- [ ] **Verify toolbar** displays all expected buttons
- [ ] **Verify status bar** shows frame count, rerecord count, etc.

### 2.2 Frame List View

- [ ] **Empty movie** - Frame list shows at least frame 0
- [ ] **Scroll** - Can scroll through frames with mouse wheel
- [ ] **Select single frame** - Click to select a frame
- [ ] **Select multiple frames** - Shift+Click for range selection
- [ ] **Verify columns** - Frame number, P1/P2 input columns display

### 2.3 Input Editing

#### 2.3.1 Button Toggle

- [ ] **Click button cell** to toggle ON
- [ ] **Click again** to toggle OFF
- [ ] **Visual feedback** - ON state clearly visible (color/icon)
- [ ] **All buttons** - Test each button for the system (A, B, Start, Select, D-pad)

#### 2.3.2 D-Pad Input

- [ ] **Up** toggles correctly
- [ ] **Down** toggles correctly
- [ ] **Left** toggles correctly
- [ ] **Right** toggles correctly
- [ ] **Diagonal** - Can set Up+Left, Up+Right, Down+Left, Down+Right
- [ ] **Opposite directions** - Verify behavior when setting Up+Down or Left+Right

### 2.4 Frame Operations

#### 2.4.1 Insert Frames

- [ ] **Insert single frame** - Select frame, Edit → Insert Frame (or Ctrl+Shift+I)
- [ ] **Insert multiple frames** - Insert multiple frames at once
- [ ] **Insert at beginning** - Insert at frame 0
- [ ] **Insert at end** - Insert after last frame
- [ ] **Verify movie length increases**

#### 2.4.2 Delete Frames

- [ ] **Delete single frame** - Select frame, Edit → Delete Frame (or Del)
- [ ] **Delete multiple frames** - Select range and delete
- [ ] **Delete at beginning** - Delete frame 0
- [ ] **Verify movie length decreases**

#### 2.4.3 Copy/Paste

- [ ] **Copy single frame** - Ctrl+C
- [ ] **Paste frame** - Ctrl+V
- [ ] **Cut frame** - Ctrl+X
- [ ] **Paste preserves input** - Verify pasted frame has same inputs
- [ ] **Copy multiple frames** - Select range and copy
- [ ] **Paste multiple frames** - Verify all frames paste correctly

### 2.5 Undo/Redo

- [ ] **Undo insert** - Ctrl+Z after inserting frame
- [ ] **Undo delete** - Ctrl+Z after deleting frame
- [ ] **Undo input change** - Ctrl+Z after toggling button
- [ ] **Redo** - Ctrl+Y or Ctrl+Shift+Z restores action
- [ ] **Multiple undo** - Can undo multiple operations
- [ ] **Undo limit** - Verify behavior at undo history limit

### 2.6 Playback Controls

#### 2.6.1 Basic Playback

- [ ] **Play** - Start playback from current frame
- [ ] **Pause** - Pause playback
- [ ] **Stop** - Stop and return to frame 0
- [ ] **Playback follows movie** - Inputs apply to emulation

#### 2.6.2 Frame-by-Frame

- [ ] **Frame Advance** - Single step forward (F key or button)
- [ ] **Frame Rewind** - Single step backward (Shift+F or button)
- [ ] **Frame advance at end** - Verify behavior at last frame

#### 2.6.3 Speed Control

- [ ] **0.25x speed** - Playback at quarter speed
- [ ] **0.5x speed** - Playback at half speed
- [ ] **1x speed** - Normal speed
- [ ] **2x speed** - Double speed
- [ ] **4x speed** - Quadruple speed

### 2.7 Seeking

- [ ] **Click frame to seek** - Double-click frame to seek
- [ ] **Seek forward** - Seek from frame 0 to frame 1000
- [ ] **Seek backward** - Seek from frame 1000 to frame 500
- [ ] **Seek to beginning** - Seek to frame 0
- [ ] **Seek uses greenzone** - Fast seeking via savestates

### 2.8 Piano Roll View

- [ ] **Toggle piano roll** - View → Piano Roll (or button)
- [ ] **Visual timeline displays** - Shows button lanes
- [ ] **Click cell** to toggle input
- [ ] **Paint cells** - Click and drag to paint inputs
- [ ] **Zoom in** - Increase cell size
- [ ] **Zoom out** - Decrease cell size
- [ ] **Selection highlighting** - Selected range is visible
- [ ] **Playback cursor** - Current frame indicator visible
- [ ] **Greenzone indicator** - Saved frames are marked
- [ ] **Lag frame indicator** - Lag frames marked if applicable

### 2.9 Recording Mode

#### 2.9.1 Start/Stop Recording

- [ ] **Start recording** - Recording → Start Recording
- [ ] **Recording indicator** - Status bar shows "REC" or similar
- [ ] **Stop recording** - Recording → Stop Recording
- [ ] **Inputs recorded** - Controller inputs appear in movie

#### 2.9.2 Recording Modes

- [ ] **Append mode** - New inputs added after current frame
- [ ] **Insert mode** - New inputs inserted at current frame
- [ ] **Overwrite mode** - New inputs replace existing frames

#### 2.9.3 Rerecording

- [ ] **Rerecord from savestate** - Load state and continue recording
- [ ] **Rerecord count increases** - Counter increments on rerecord
- [ ] **Status bar shows rerecord count**

### 2.10 Branch Management

- [ ] **Create branch** - Recording → Create Branch
- [ ] **Branch appears in list**
- [ ] **Name branch** - Can rename branch
- [ ] **Load branch** - Switch to different branch
- [ ] **Delete branch** - Remove unwanted branch
- [ ] **Branch preserves greenzone** - Savestates maintained

### 2.11 Greenzone System

- [ ] **Automatic savestate capture** - States saved periodically
- [ ] **Greenzone count displays** - Status shows "X savestates"
- [ ] **Memory usage displays** - Shows MB used
- [ ] **Fast seeking works** - Seeking uses nearest savestate
- [ ] **Greenzone pruning** - Old states removed when limit reached
- [ ] **Compression** - Older states are compressed

### 2.12 Multi-Controller Support

- [ ] **Player 1 inputs** - P1 column editable
- [ ] **Player 2 inputs** - P2 column editable (if applicable)
- [ ] **Controller layout switching** - Can change system type
- [ ] **NES layout** - A, B, Select, Start, D-pad
- [ ] **SNES layout** - A, B, X, Y, L, R, Select, Start, D-pad
- [ ] **Game Boy layout** - A, B, Select, Start, D-pad

### 2.13 File Operations

- [ ] **New movie** - File → New Movie
- [ ] **Save movie** - File → Save (Ctrl+S)
- [ ] **Save As** - File → Save As (Ctrl+Shift+S)
- [ ] **Open movie** - File → Open Movie (Ctrl+O)
- [ ] **Close TAS Editor** - File → Close (confirm save prompt)

---

## 3. Save State System Testing

### 3.1 Quick Save/Load Slots

#### 3.1.1 Keyboard Shortcuts

- [ ] **Shift+F1** - Save to slot 1
- [ ] **F1** - Load from slot 1
- [ ] **Shift+F2 through Shift+F10** - Save to slots 2-10
- [ ] **F2 through F10** - Load from slots 2-10
- [ ] **Visual feedback** - Message displays on save/load

#### 3.1.2 Save State Contents

- [ ] **Game state restored** - Position, health, items correct
- [ ] **Video state** - Screen displays correctly after load
- [ ] **Audio state** - No audio glitches after load
- [ ] **All systems** - Test on each supported system

### 3.2 Save State Browser

- [ ] **Open browser** - File → Save States or shortcut
- [ ] **Grid view** - Thumbnails display
- [ ] **Timestamps** - Creation time visible
- [ ] **Select state** - Click to select
- [ ] **Load selected** - Load button or double-click
- [ ] **Delete state** - Remove unwanted saves
- [ ] **Rename state** - Can add custom name

### 3.3 Thumbnail Previews

- [ ] **Thumbnails generate** - Screenshot captured with save
- [ ] **Thumbnails display** - Visible in browser
- [ ] **Correct image** - Thumbnail matches save point
- [ ] **Performance** - No lag when generating thumbnails

### 3.4 Per-Game Organization

- [ ] **Saves organized by ROM** - Each game has own folder
- [ ] **Different ROMs** - Saves don't interfere
- [ ] **ROM hash matching** - Correct saves load for ROM

### 3.5 State File Format

- [ ] **Compression** - States are reasonably sized
- [ ] **Compatibility** - States can be shared (if applicable)
- [ ] **Version check** - Incompatible states show error

---

## 4. Movie Converter Testing

### 4.1 Supported Formats

Test import and export for each format:

#### 4.1.1 FM2 (FCEUX)

- [ ] **Import FM2** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export FM2** - File → Export Movie
- [ ] **Roundtrip** - Import → Export → Import unchanged

#### 4.1.2 SMV (Snes9x)

- [ ] **Import SMV** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export SMV** - File → Export Movie
- [ ] **Roundtrip** - Import → Export → Import unchanged

#### 4.1.3 BK2 (BizHawk)

- [ ] **Import BK2** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export BK2** - File → Export Movie
- [ ] **Roundtrip** - Import → Export → Import unchanged

#### 4.1.4 LSMV (lsnes)

- [ ] **Import LSMV** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export LSMV** - File → Export Movie
- [ ] **Roundtrip** - Import → Export → Import unchanged

#### 4.1.5 VBM (VisualBoyAdvance)

- [ ] **Import VBM** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export VBM** - File → Export Movie

#### 4.1.6 GMV (Gens)

- [ ] **Import GMV** - File → Import Movie
- [ ] **Verify inputs** - Playback matches original
- [ ] **Export GMV** - File → Export Movie

#### 4.1.7 Nexen Movie Format (.nmv)

- [ ] **Save as NMV** - File → Save
- [ ] **Load NMV** - File → Open
- [ ] **All metadata preserved** - Author, comments, rerecord count

### 4.2 Metadata Handling

- [ ] **Author preserved** - Import/export keeps author name
- [ ] **Comments preserved** - Import/export keeps comments
- [ ] **Rerecord count** - Count is preserved or converted
- [ ] **System type** - Correct system detected
- [ ] **ROM hash** - Hash matches or warns

### 4.3 Error Handling

- [ ] **Invalid file** - Error message for corrupt files
- [ ] **Wrong system** - Error for wrong system movie
- [ ] **Missing ROM** - Warning if ROM not loaded

---

## 5. Pansy Export Testing

### 5.1 Basic Export

- [ ] **Export menu item** - Debug → Export Pansy Metadata
- [ ] **File dialog appears** - Can choose save location
- [ ] **File saves** - .pansy file created
- [ ] **No crash** - Export completes successfully

### 5.2 Exported Data

#### 5.2.1 Symbols

- [ ] **Labels exported** - Named addresses in file
- [ ] **Label names correct** - Match debugger labels
- [ ] **Address values correct** - Addresses are accurate

#### 5.2.2 Comments

- [ ] **Comments exported** - Inline comments in file
- [ ] **Comment text correct** - Matches debugger comments

#### 5.2.3 Code/Data Markers

- [ ] **Code regions** - Marked as code
- [ ] **Data regions** - Marked as data
- [ ] **CDL integration** - Uses Code/Data Logger info

#### 5.2.4 Cross-References

- [ ] **Jump targets** - Branch destinations marked
- [ ] **Call targets** - Subroutine entries marked
- [ ] **Data references** - Memory access targets

### 5.3 Platform-Specific Export

- [ ] **NES export** - Correct 6502 format
- [ ] **SNES export** - Correct 65816 format
- [ ] **Game Boy export** - Correct LR35902 format
- [ ] **GBA export** - Correct ARM7 format

### 5.4 Large Project Export

- [ ] **Many labels** - 1000+ labels exports correctly
- [ ] **Performance** - Export completes in reasonable time
- [ ] **File size** - Output is reasonable size

---

## 6. Keyboard Shortcuts Testing

### 6.1 Global Shortcuts

- [ ] **F5** - Run/Pause emulation
- [ ] **F6** - Reset game
- [ ] **Esc** - Pause emulation
- [ ] **F12** - Screenshot

### 6.2 Save State Shortcuts

- [ ] **Shift+F1-F10** - Save to slot 1-10
- [ ] **F1-F10** - Load from slot 1-10
- [ ] **Ctrl+S** - Save state (browser)
- [ ] **Ctrl+L** - Load state (browser)

### 6.3 TAS Editor Shortcuts

- [ ] **Ctrl+N** - New movie
- [ ] **Ctrl+O** - Open movie
- [ ] **Ctrl+S** - Save movie
- [ ] **Ctrl+Z** - Undo
- [ ] **Ctrl+Y** - Redo
- [ ] **Ctrl+C** - Copy frames
- [ ] **Ctrl+V** - Paste frames
- [ ] **Ctrl+X** - Cut frames
- [ ] **Del** - Delete frames
- [ ] **Space** - Play/Pause
- [ ] **F** - Frame advance
- [ ] **Shift+F** - Frame rewind

### 6.4 Debugger Shortcuts

- [ ] **F9** - Toggle breakpoint
- [ ] **F10** - Step over
- [ ] **F11** - Step into
- [ ] **Shift+F11** - Step out

### 6.5 Shortcut Conflicts

- [ ] **No conflicts** - Each shortcut has one function
- [ ] **Context-aware** - Shortcuts work in correct context

---

## 7. UI/UX Testing

### 7.1 Window Management

- [ ] **Resize main window** - Smooth resizing
- [ ] **Minimize/restore** - Works correctly
- [ ] **Maximize** - Fills screen properly
- [ ] **Multiple monitors** - Works on different displays

### 7.2 Menu System

- [ ] **File menu** - All items functional
- [ ] **Edit menu** - All items functional
- [ ] **View menu** - All items functional
- [ ] **Tools menu** - All items functional
- [ ] **Debug menu** - All items functional
- [ ] **Help menu** - About dialog works

### 7.3 Toolbar

- [ ] **All buttons visible** - Icons display correctly
- [ ] **Tooltips** - Hover shows description
- [ ] **Button states** - Enabled/disabled correctly

### 7.4 Status Bar

- [ ] **Frame counter** - Shows current frame
- [ ] **FPS display** - Shows framerate
- [ ] **System info** - Shows loaded ROM/system

### 7.5 Dialogs

- [ ] **Settings dialog** - Opens and saves correctly
- [ ] **File dialogs** - Open/Save work properly
- [ ] **Confirmation dialogs** - Appear when needed
- [ ] **Error dialogs** - Show useful messages

### 7.6 Themes/Appearance

- [ ] **Default theme** - Looks correct
- [ ] **High contrast** - Text is readable
- [ ] **Font scaling** - Text scales with system settings

---

## 8. Multi-System Testing

### 8.1 NES Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond
- [ ] **Save states** - Save/load works
- [ ] **TAS Editor** - Recording works

### 8.2 SNES Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond
- [ ] **Save states** - Save/load works
- [ ] **TAS Editor** - Recording works
- [ ] **Enhancement chips** - Super FX, SA-1, DSP work

### 8.3 Game Boy Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond
- [ ] **Save states** - Save/load works
- [ ] **TAS Editor** - Recording works

### 8.4 Game Boy Color Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Colors** - Palette displays correctly
- [ ] **GBC-only games** - Work properly
- [ ] **Dual-mode games** - Can choose mode

### 8.5 Game Boy Advance Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - All buttons work (L/R triggers)
- [ ] **Save states** - Save/load works
- [ ] **TAS Editor** - Recording works

### 8.6 Master System Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond
- [ ] **Save states** - Save/load works

### 8.7 PC Engine Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond
- [ ] **Save states** - Save/load works

### 8.8 WonderSwan Testing

- [ ] **ROM loads** - Game starts correctly
- [ ] **Graphics** - No visual glitches
- [ ] **Audio** - Sound plays correctly
- [ ] **Input** - Controls respond (vertical/horizontal modes)
- [ ] **Save states** - Save/load works

---

## 9. Performance Testing

### 9.1 Frame Rate

- [ ] **60 FPS maintained** - NTSC games run at 60 FPS
- [ ] **50 FPS maintained** - PAL games run at 50 FPS
- [ ] **No frame drops** - Smooth gameplay
- [ ] **Fast forward** - High speeds achievable

### 9.2 Memory Usage

- [ ] **Base memory** - Reasonable idle memory
- [ ] **Greenzone memory** - Stays within limit
- [ ] **No memory leaks** - Memory stable over time
- [ ] **Large movies** - Handle 100,000+ frames

### 9.3 CPU Usage

- [ ] **Idle usage** - Low CPU when paused
- [ ] **Active usage** - Reasonable during emulation
- [ ] **Debugger impact** - Minimal slowdown with debugger

### 9.4 Startup Time

- [ ] **Application launch** - Starts within 3 seconds
- [ ] **ROM load time** - Games load quickly
- [ ] **TAS Editor open** - Opens promptly

---

## 10. Build & Platform Testing

### 10.1 Windows x64

- [ ] **Download** - Can download from release
- [ ] **Extract/Run** - Executable runs
- [ ] **Features** - All features work
- [ ] **Windows 10** - Works on Win10
- [ ] **Windows 11** - Works on Win11

### 10.2 Linux x64

- [ ] **Download** - Can download from release
- [ ] **AppImage runs** - chmod +x and execute
- [ ] **Features** - All features work
- [ ] **Dependencies** - No missing libraries

### 10.3 Linux ARM64

- [ ] **Download** - Can download from release
- [ ] **AppImage runs** - chmod +x and execute
- [ ] **Features** - All features work
- [ ] **Raspberry Pi** - Works on Pi 4/5 (if applicable)

### 10.4 macOS Intel (x64)

- [ ] **Download** - Can download from release
- [ ] **Unzip and run** - App launches
- [ ] **Gatekeeper** - Handles security prompt
- [ ] **Features** - All features work

### 10.5 macOS Apple Silicon (ARM64)

- [ ] **Download** - Can download from release
- [ ] **Unzip and run** - App launches natively
- [ ] **Features** - All features work
- [ ] **Performance** - Good performance on M1/M2/M3

---

## Test Results Summary

### Overall Status

| Section | Pass | Fail | Not Tested |
|---------|------|------|------------|
| TAS Editor | | | |
| Save States | | | |
| Movie Converter | | | |
| Pansy Export | | | |
| Keyboard Shortcuts | | | |
| UI/UX | | | |
| Multi-System | | | |
| Performance | | | |
| Build/Platform | | | |

### Critical Issues Found

1. (List any critical bugs here)

### Non-Critical Issues Found

1. (List any minor bugs here)

### Notes and Observations

(Add any testing notes here)

---

## Test Sign-Off

| Tester | Date | Sections Tested | Result |
|--------|------|-----------------|--------|
| | | | |

---

*Document version: 1.0.0*
*Last updated: 2026-02-02*
