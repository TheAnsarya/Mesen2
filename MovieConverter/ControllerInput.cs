namespace Nexen.MovieConverter;

/// <summary>
/// Input state for a single controller on a single frame.
/// Supports standard gamepads and special input devices.
/// </summary>
public class ControllerInput {
	// ========== Standard SNES/NES Buttons ==========

	/// <summary>A button (right face button)</summary>
	public bool A { get; set; }

	/// <summary>B button (bottom face button)</summary>
	public bool B { get; set; }

	/// <summary>X button (top face button, SNES only)</summary>
	public bool X { get; set; }

	/// <summary>Y button (left face button, SNES only)</summary>
	public bool Y { get; set; }

	/// <summary>L shoulder button (SNES/GBA)</summary>
	public bool L { get; set; }

	/// <summary>R shoulder button (SNES/GBA)</summary>
	public bool R { get; set; }

	/// <summary>Start button</summary>
	public bool Start { get; set; }

	/// <summary>Select button</summary>
	public bool Select { get; set; }

	/// <summary>D-pad Up</summary>
	public bool Up { get; set; }

	/// <summary>D-pad Down</summary>
	public bool Down { get; set; }

	/// <summary>D-pad Left</summary>
	public bool Left { get; set; }

	/// <summary>D-pad Right</summary>
	public bool Right { get; set; }

	// ========== Mouse/Pointer Input ==========

	/// <summary>Mouse X position (for Mouse, Super Scope, etc.)</summary>
	public int? MouseX { get; set; }

	/// <summary>Mouse Y position</summary>
	public int? MouseY { get; set; }

	/// <summary>Primary mouse button / trigger</summary>
	public bool? MouseButton1 { get; set; }

	/// <summary>Secondary mouse button / cursor</summary>
	public bool? MouseButton2 { get; set; }

	// ========== Controller Type ==========

	/// <summary>The type of controller for this port</summary>
	public ControllerType Type { get; set; } = ControllerType.Gamepad;

	// ========== Format Conversion ==========

	/// <summary>
	/// Check if any button is pressed
	/// </summary>
	public bool HasInput => A || B || X || Y || L || R || Start || Select ||
							Up || Down || Left || Right ||
							MouseButton1 == true || MouseButton2 == true;

	/// <summary>
	/// Convert to Nexen/Mesen text format: BYsSUDLRAXLR
	/// </summary>
	/// <returns>12-character string representing button states</returns>
	public string ToNexenFormat() {
		if (Type == ControllerType.None) {
			return "............";
		}

		Span<char> chars = stackalloc char[12];
		chars[0] = B ? 'B' : '.';
		chars[1] = Y ? 'Y' : '.';
		chars[2] = Select ? 's' : '.';
		chars[3] = Start ? 'S' : '.';
		chars[4] = Up ? 'U' : '.';
		chars[5] = Down ? 'D' : '.';
		chars[6] = Left ? 'L' : '.';
		chars[7] = Right ? 'R' : '.';
		chars[8] = A ? 'A' : '.';
		chars[9] = X ? 'X' : '.';
		chars[10] = L ? 'l' : '.';
		chars[11] = R ? 'r' : '.';

		return new string(chars);
	}

	/// <summary>
	/// Parse from Nexen/Mesen text format
	/// </summary>
	/// <param name="input">12-character input string</param>
	/// <returns>Parsed ControllerInput</returns>
	public static ControllerInput FromNexenFormat(ReadOnlySpan<char> input) {
		var ctrl = new ControllerInput { Type = ControllerType.Gamepad };

		if (input.Length < 12) {
			return ctrl;
		}

		// Nexen format: BYsSUDLRAXLR
		ctrl.B = input[0] != '.';
		ctrl.Y = input[1] != '.';
		ctrl.Select = input[2] != '.';
		ctrl.Start = input[3] != '.';
		ctrl.Up = input[4] != '.';
		ctrl.Down = input[5] != '.';
		ctrl.Left = input[6] != '.';
		ctrl.Right = input[7] != '.';
		ctrl.A = input[8] != '.';
		ctrl.X = input[9] != '.';
		ctrl.L = input[10] != '.';
		ctrl.R = input[11] != '.';

		return ctrl;
	}

	/// <summary>
	/// Convert to SMV 2-byte binary format
	/// </summary>
	/// <returns>16-bit value representing button states</returns>
	public ushort ToSmvFormat() {
		ushort value = 0;

		// SMV bit layout (SNES):
		// Low byte:  ----RLXA
		// High byte: BYsS UDLR

		if (R) value |= 0x10;
		if (L) value |= 0x20;
		if (X) value |= 0x40;
		if (A) value |= 0x80;
		if (Right) value |= 0x0100;
		if (Left) value |= 0x0200;
		if (Down) value |= 0x0400;
		if (Up) value |= 0x0800;
		if (Start) value |= 0x1000;
		if (Select) value |= 0x2000;
		if (Y) value |= 0x4000;
		if (B) value |= 0x8000;

		return value;
	}

	/// <summary>
	/// Parse from SMV 2-byte binary format
	/// </summary>
	/// <param name="value">16-bit input value</param>
	/// <returns>Parsed ControllerInput</returns>
	public static ControllerInput FromSmvFormat(ushort value) {
		return new ControllerInput {
			Type = ControllerType.Gamepad,
			R = (value & 0x10) != 0,
			L = (value & 0x20) != 0,
			X = (value & 0x40) != 0,
			A = (value & 0x80) != 0,
			Right = (value & 0x0100) != 0,
			Left = (value & 0x0200) != 0,
			Down = (value & 0x0400) != 0,
			Up = (value & 0x0800) != 0,
			Start = (value & 0x1000) != 0,
			Select = (value & 0x2000) != 0,
			Y = (value & 0x4000) != 0,
			B = (value & 0x8000) != 0
		};
	}

	/// <summary>
	/// Convert to LSMV text format (similar to Nexen but case-insensitive)
	/// </summary>
	/// <returns>12-character string</returns>
	public string ToLsmvFormat() {
		Span<char> chars = stackalloc char[12];
		chars[0] = B ? 'B' : '.';
		chars[1] = Y ? 'Y' : '.';
		chars[2] = Select ? 's' : '.';
		chars[3] = Start ? 'S' : '.';
		chars[4] = Up ? 'u' : '.';
		chars[5] = Down ? 'd' : '.';
		chars[6] = Left ? 'l' : '.';
		chars[7] = Right ? 'r' : '.';
		chars[8] = A ? 'A' : '.';
		chars[9] = X ? 'X' : '.';
		chars[10] = L ? 'L' : '.';
		chars[11] = R ? 'R' : '.';

		return new string(chars);
	}

	/// <summary>
	/// Parse from LSMV text format
	/// </summary>
	/// <param name="input">Input string</param>
	/// <returns>Parsed ControllerInput</returns>
	public static ControllerInput FromLsmvFormat(ReadOnlySpan<char> input) {
		var ctrl = new ControllerInput { Type = ControllerType.Gamepad };

		if (input.Length < 12) {
			return ctrl;
		}

		// LSMV format: BYsSudlrAXLR (case variations)
		ctrl.B = char.ToUpperInvariant(input[0]) == 'B';
		ctrl.Y = char.ToUpperInvariant(input[1]) == 'Y';
		ctrl.Select = char.ToLowerInvariant(input[2]) == 's';
		ctrl.Start = char.ToUpperInvariant(input[3]) == 'S';
		ctrl.Up = char.ToLowerInvariant(input[4]) == 'u';
		ctrl.Down = char.ToLowerInvariant(input[5]) == 'd';
		ctrl.Left = char.ToLowerInvariant(input[6]) == 'l';
		ctrl.Right = char.ToLowerInvariant(input[7]) == 'r';
		ctrl.A = char.ToUpperInvariant(input[8]) == 'A';
		ctrl.X = char.ToUpperInvariant(input[9]) == 'X';
		ctrl.L = char.ToUpperInvariant(input[10]) == 'L';
		ctrl.R = char.ToUpperInvariant(input[11]) == 'R';

		return ctrl;
	}

	/// <summary>
	/// Convert to FM2 text format (NES): RLDUSTBA
	/// </summary>
	/// <returns>8-character string for NES input</returns>
	public string ToFm2Format() {
		Span<char> chars = stackalloc char[8];
		chars[0] = Right ? 'R' : '.';
		chars[1] = Left ? 'L' : '.';
		chars[2] = Down ? 'D' : '.';
		chars[3] = Up ? 'U' : '.';
		chars[4] = Start ? 'T' : '.';  // FM2 uses T for Start
		chars[5] = Select ? 'S' : '.';
		chars[6] = B ? 'B' : '.';
		chars[7] = A ? 'A' : '.';

		return new string(chars);
	}

	/// <summary>
	/// Parse from FM2 text format (NES)
	/// </summary>
	/// <param name="input">8-character input string</param>
	/// <returns>Parsed ControllerInput</returns>
	public static ControllerInput FromFm2Format(ReadOnlySpan<char> input) {
		var ctrl = new ControllerInput { Type = ControllerType.Gamepad };

		if (input.Length < 8) {
			return ctrl;
		}

		// FM2 format: RLDUSTBA
		ctrl.Right = input[0] != '.';
		ctrl.Left = input[1] != '.';
		ctrl.Down = input[2] != '.';
		ctrl.Up = input[3] != '.';
		ctrl.Start = input[4] != '.';
		ctrl.Select = input[5] != '.';
		ctrl.B = input[6] != '.';
		ctrl.A = input[7] != '.';

		return ctrl;
	}

	/// <summary>
	/// Create a deep copy of this input
	/// </summary>
	/// <returns>New ControllerInput with same values</returns>
	public ControllerInput Clone() {
		return new ControllerInput {
			A = A, B = B, X = X, Y = Y,
			L = L, R = R,
			Start = Start, Select = Select,
			Up = Up, Down = Down, Left = Left, Right = Right,
			MouseX = MouseX, MouseY = MouseY,
			MouseButton1 = MouseButton1, MouseButton2 = MouseButton2,
			Type = Type
		};
	}

	public override string ToString() => ToNexenFormat();
}
