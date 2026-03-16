# Save States Guide

## Overview

Nexen supports quick save workflows, a visual save browser, designated slots, and per-game organization.

## Common Workflows

### Quick Save and Quick Load

1. Press `F1` to create a timestamped quick save.
2. Press `Shift+F1` to open the save browser.
3. Select a state thumbnail and load it.

### Designated Slot

1. Press `Shift+F4` to save to the designated slot.
2. Press `F4` to load the designated slot.

### File-Based States

1. Use `Ctrl+Shift+S` to save state to file.
2. Use `Ctrl+L` to load state from file.

## GUI Tips

- Use the visual picker to compare multiple branches of play.
- Use timestamped entries for rapid iteration while testing.
- Keep per-game save sets to avoid cross-ROM confusion.

## Panel Walkthrough (Screenshot Anchors)

### Walkthrough A: Quick Save and Visual Browser

1. In the main emulation window, press `F1` to create a quick save.
2. Press `Shift+F1` to open the Save State Browser panel.
3. In the browser grid, select a thumbnail with the target timestamp.
4. Confirm load from the selected entry.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Main Emulation Window | `save-states-a-01-main-window` | `docs/screenshots/save-states/a-01-main-window.png` |
| 2 | Save State Browser (Grid View) | `save-states-a-02-browser-grid` | `docs/screenshots/save-states/a-02-browser-grid.png` |
| 3 | Save State Browser (Selected Entry) | `save-states-a-03-selected-entry` | `docs/screenshots/save-states/a-03-selected-entry.png` |

### Walkthrough B: Designated Slot Workflow

1. Press `Shift+F4` from gameplay to store current state in designated slot.
2. Continue play, then press `F4` to load the designated slot.
3. Verify game state returns to the exact checkpoint.

| Step | Panel | Screenshot Anchor ID | Capture Target |
|---|---|---|---|
| 1 | Main Emulation Window (Before Slot Save) | `save-states-b-01-before-slot-save` | `docs/screenshots/save-states/b-01-before-slot-save.png` |
| 2 | Main Emulation Window (After Slot Load) | `save-states-b-02-after-slot-load` | `docs/screenshots/save-states/b-02-after-slot-load.png` |

## Related Links

- [Documentation Index](README.md)
- [Rewind Guide](Rewind.md)
- [Movie System Guide](Movie-System.md)
