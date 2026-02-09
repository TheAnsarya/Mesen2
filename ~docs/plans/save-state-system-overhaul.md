# Save State System Overhaul

## Overview

This document outlines the comprehensive overhaul of the Nexen save state system to provide better organization, visual identification, and automatic backup capabilities.

**Epic Issue**: #172
**Related Issues**: #173, #174, #175, #176, #177

## Goals

1. **Origin Tracking** - Know where each save came from
2. **Recent Play Queue** - 5-minute interval rotating saves (1 hour retention)
3. **Quick Save Auto-Save** - 20-30 minute auto-save to quick save slots
4. **Visual Badges** - Bootstrap-style colored badges for save origins
5. **Better UI** - Dedicated menu items and picker for recent saves

---

## SaveStateOrigin Enum

```csharp
public enum SaveStateOrigin : byte
{
	Auto = 0,   // Blue badge - System auto-saves (20-30 min)
	Save = 1,   // Green badge - User-initiated saves
	Recent = 2, // Red badge - Recent play queue (5 min)
	Lua = 3     // Yellow badge - Lua script saves
}
```

### Badge Colors (Bootstrap 5)

| Origin | Color | Hex Code | Text Color |
|--------|-------|----------|------------|
| Auto   | Blue  | #0d6efd  | White      |
| Save   | Green | #198754  | White      |
| Recent | Red   | #dc3545  | White      |
| Lua    | Yellow| #ffc107  | Black      |

---

## Save State Categories

### 1. Quick Saves (Slots 1-10)

**Existing functionality** with enhancements:

- User-initiated via Shift+F1-F10
- Origin: `Save` (green badge)
- **NEW**: Auto-saved every 20-30 minutes
  - Origin: `Auto` (blue badge)
  - Cycles through slots
  - Avoids overwriting recent user saves

### 2. Recent Play Queue (Slots 12-23)

**New feature** - rotating queue:

- Automatic every 5 minutes
- 12 saves = 1 hour retention
- Origin: `Recent` (red badge)
- FIFO rotation (oldest replaced)
- Separate from quick saves
- Accessible via:
  - File → Recent Play submenu
  - Dedicated keyboard shortcuts

### 3. Auto Save (Slot 11)

**Existing** - system auto-save slot:

- Configurable interval (default: 20 minutes)
- Origin: `Auto` (blue badge)
- Used for last-session recovery

### 4. Lua Saves

**Existing** - script-created saves:

- Origin: `Lua` (yellow badge)
- No automatic creation

---

## File Format Changes

### nexen-save Header Extension

Add origin byte after existing header fields:

```
Offset | Size | Field
-------|------|-------
...    | ...  | (existing fields)
+N     | 1    | SaveStateOrigin (uint8_t)
```

### Backwards Compatibility

- Existing saves without origin default to `Save` (1)
- Version check in header for new field
- Old versions ignore new field

---

## Slot Allocation

```
Slot  | Purpose        | Origin
------|----------------|--------
1-10  | Quick Saves    | Save/Auto
11    | Auto Save      | Auto
12-23 | Recent Play    | Recent
24+   | Reserved       | -
```

---

## Configuration Options

### PreferencesConfig

```csharp
// Quick Save Auto-Save (20-30 min)
public bool EnableQuickSaveAutoSave { get; set; } = true;
public uint QuickSaveAutoSaveInterval { get; set; } = 20; // minutes

// Recent Play Queue (5 min)
public bool EnableRecentPlaySaves { get; set; } = true;
public uint RecentPlayInterval { get; set; } = 5; // minutes
public uint RecentPlayRetentionCount { get; set; } = 12; // saves
```

---

## Menu Structure

### File Menu

```
File
├── ...
├── Save State
│   ├── Quick Save (Shift+F1)
│   ├── Quick Save Slot... (submenu)
│   └── Browse Save States... (F1)
├── Load State
│   ├── Quick Load (Shift+F5)
│   ├── Quick Load Slot... (submenu)
│   └── Browse Save States... (F5)
├── Recent Play
│   ├── Load Most Recent
│   ├── ────────────────
│   ├── 5 min ago - World 3-1
│   ├── 10 min ago - World 2-4
│   ├── ...
│   └── Browse Recent Play...
├── ...
```

### Settings Menu

```
Settings
├── ...
├── Auto Save
│   ├── ✓ Auto Save States
│   ├── ✓ Auto Save to Quick Slots (20 min)
│   ├── ✓ Recent Play Saves (5 min)
│   ├── ────────────────
│   ├── ✓ Auto Save Pansy Data
│   └── ✓ Save Pansy on ROM Unload
├── ────────────────
└── Preferences
```

---

## UI Components

### SaveStateBadge Control

```xml
<Border Classes="save-state-badge auto">
	<TextBlock Text="AUTO"/>
</Border>
```

### Badge Styles

```xml
<Style Selector="Border.save-state-badge">
	<Setter Property="CornerRadius" Value="10"/>
	<Setter Property="Padding" Value="6,2"/>
</Style>
<Style Selector="Border.save-state-badge.auto">
	<Setter Property="Background" Value="#0d6efd"/>
</Style>
<Style Selector="Border.save-state-badge.save">
	<Setter Property="Background" Value="#198754"/>
</Style>
<Style Selector="Border.save-state-badge.recent">
	<Setter Property="Background" Value="#dc3545"/>
</Style>
<Style Selector="Border.save-state-badge.lua">
	<Setter Property="Background" Value="#ffc107"/>
</Style>
```

---

## Implementation Order

1. **Phase 1**: SaveStateOrigin enum and file format (#173)
2. **Phase 2**: Recent Play queue infrastructure (#174)
3. **Phase 3**: Quick Save auto-save (#175)
4. **Phase 4**: Recent Play UI (#176)
5. **Phase 5**: Origin badges (#177)

---

## C++ Core Changes

### SaveStateManager.h

```cpp
enum class SaveStateOrigin : uint8_t {
	Auto = 0,
	Save = 1,
	Recent = 2,
	Lua = 3
};

class SaveStateManager {
public:
	// Existing
	static constexpr uint32_t AutoSaveStateIndex = 11;
	
	// New
	static constexpr uint32_t RecentPlayStartIndex = 12;
	static constexpr uint32_t RecentPlayEndIndex = 23;
	static constexpr uint32_t RecentPlayCount = 12;
	
	void SaveState(uint32_t slot, bool displayMessage, SaveStateOrigin origin);
	SaveStateOrigin GetStateOrigin(uint32_t slot);
	
private:
	void ProcessRecentPlaySave();
	void ProcessQuickSaveAutoSave();
	uint32_t _recentPlayNextSlot = RecentPlayStartIndex;
};
```

---

## Risk Assessment

| Risk | Impact | Mitigation |
|------|--------|------------|
| File format change breaks existing saves | High | Version check, defaults |
| Performance impact from frequent saves | Medium | Async I/O, compression |
| Disk space usage | Low | Configurable retention |

---

## Timeline

- **Week 1**: #173 (enum/format), #174 (recent play core)
- **Week 2**: #175 (quick save auto), #176 (UI)
- **Week 3**: #177 (badges), testing, polish

---

## Testing Checklist

- [ ] Origin tracking persists across save/load
- [ ] Backwards compatibility with old saves
- [ ] Recent play queue rotates correctly
- [ ] Quick save auto-save respects user saves
- [ ] Badges display correctly in all themes
- [ ] Performance acceptable with frequent saves
- [ ] Memory usage within limits
