namespace Nexen.MovieConverter;

/// <summary>
/// Represents the format of a TAS movie file
/// </summary>
public enum MovieFormat {
	/// <summary>Unknown or unsupported format</summary>
	Unknown,

	/// <summary>Nexen native format (.nexen-movie) - ZIP-based, human-readable</summary>
	Nexen,

	/// <summary>Mesen/Mesen2 format (.mmm/.mmo)</summary>
	Mesen,

	/// <summary>Snes9x format (.smv)</summary>
	Smv,

	/// <summary>lsnes format (.lsmv) - ZIP-based</summary>
	Lsmv,

	/// <summary>FCEUX format (.fm2) - text-based, NES only</summary>
	Fm2,

	/// <summary>BizHawk format (.bk2) - ZIP-based, multi-system</summary>
	Bk2,

	/// <summary>VisualBoyAdvance format (.vbm)</summary>
	Vbm,

	/// <summary>Gens format (.gmv) - Genesis</summary>
	Gmv,

	/// <summary>Mednafen format (.mcm)</summary>
	Mcm
}

/// <summary>
/// Target system/console type
/// </summary>
public enum SystemType {
	/// <summary>Nintendo Entertainment System (Famicom)</summary>
	Nes,

	/// <summary>Super Nintendo Entertainment System (Super Famicom)</summary>
	Snes,

	/// <summary>Game Boy (Original)</summary>
	Gb,

	/// <summary>Game Boy Color</summary>
	Gbc,

	/// <summary>Game Boy Advance</summary>
	Gba,

	/// <summary>Sega Master System</summary>
	Sms,

	/// <summary>Sega Genesis (Mega Drive)</summary>
	Genesis,

	/// <summary>PC Engine (TurboGrafx-16)</summary>
	Pce,

	/// <summary>WonderSwan</summary>
	Ws,

	/// <summary>Other or unknown system</summary>
	Other
}

/// <summary>
/// Video region/timing standard
/// </summary>
public enum RegionType {
	/// <summary>NTSC (60Hz, ~60.098814 fps for NES/SNES)</summary>
	NTSC,

	/// <summary>PAL (50Hz, ~50.006979 fps for NES/SNES)</summary>
	PAL
}

/// <summary>
/// Controller/input device type
/// </summary>
public enum ControllerType {
	/// <summary>No controller connected</summary>
	None,

	/// <summary>Standard gamepad (varies by system)</summary>
	Gamepad,

	/// <summary>Mouse (SNES Mouse, etc.)</summary>
	Mouse,

	/// <summary>Super Scope (SNES light gun)</summary>
	SuperScope,

	/// <summary>Konami Justifier (light gun)</summary>
	Justifier,

	/// <summary>Multitap (4+ player adapter)</summary>
	Multitap,

	/// <summary>Zapper (NES light gun)</summary>
	Zapper,

	/// <summary>Power Pad / Family Trainer</summary>
	PowerPad
}

/// <summary>
/// Special frame commands (reset, disk insert, etc.)
/// </summary>
[Flags]
public enum FrameCommand {
	/// <summary>Normal frame, no special command</summary>
	None = 0,

	/// <summary>Soft reset (NMI/reset button)</summary>
	SoftReset = 1,

	/// <summary>Hard reset (power cycle)</summary>
	HardReset = 2,

	/// <summary>FDS: Insert disk</summary>
	FdsInsert = 4,

	/// <summary>FDS: Select disk side</summary>
	FdsSelect = 8,

	/// <summary>VS System: Insert coin</summary>
	VsInsertCoin = 16,

	/// <summary>SNES: Controller port swap</summary>
	ControllerSwap = 32
}
