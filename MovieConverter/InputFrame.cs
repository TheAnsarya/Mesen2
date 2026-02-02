namespace Nexen.MovieConverter;

/// <summary>
/// Represents a single frame of input in a TAS movie.
/// Contains input for all controller ports plus frame metadata.
/// </summary>
public class InputFrame {
	/// <summary>
	/// Maximum number of controller ports supported
	/// </summary>
	public const int MaxPorts = 8;

	/// <summary>
	/// Frame number (0-indexed from start of movie)
	/// </summary>
	public int FrameNumber { get; set; }

	/// <summary>
	/// Input state for each controller port (up to 8 ports)
	/// </summary>
	public ControllerInput[] Controllers { get; set; } = new ControllerInput[MaxPorts];

	/// <summary>
	/// Special command for this frame (reset, disk insert, etc.)
	/// </summary>
	public FrameCommand Command { get; set; } = FrameCommand.None;

	/// <summary>
	/// Whether this frame is a lag frame (input was not polled by game)
	/// </summary>
	public bool IsLagFrame { get; set; }

	/// <summary>
	/// Optional comment/annotation for this frame (for TAS editors)
	/// </summary>
	public string? Comment { get; set; }

	/// <summary>
	/// Creates a new InputFrame with empty controller inputs
	/// </summary>
	public InputFrame() {
		for (int i = 0; i < Controllers.Length; i++) {
			Controllers[i] = new ControllerInput();
		}
	}

	/// <summary>
	/// Creates a new InputFrame with the specified frame number
	/// </summary>
	/// <param name="frameNumber">0-indexed frame number</param>
	public InputFrame(int frameNumber) : this() {
		FrameNumber = frameNumber;
	}

	/// <summary>
	/// Get input for a specific port
	/// </summary>
	/// <param name="port">Port index (0-based)</param>
	/// <returns>Controller input for that port</returns>
	public ControllerInput GetPort(int port) {
		if (port < 0 || port >= MaxPorts) {
			throw new ArgumentOutOfRangeException(nameof(port), $"Port must be 0-{MaxPorts - 1}");
		}
		return Controllers[port];
	}

	/// <summary>
	/// Check if any controller has input on this frame
	/// </summary>
	public bool HasAnyInput {
		get {
			for (int i = 0; i < Controllers.Length; i++) {
				if (Controllers[i].HasInput) {
					return true;
				}
			}
			return false;
		}
	}

	/// <summary>
	/// Convert to Nexen input log format (tab-separated ports)
	/// </summary>
	/// <param name="portCount">Number of ports to include</param>
	/// <returns>Single line for input log</returns>
	public string ToNexenLogLine(int portCount = 2) {
		var parts = new List<string>(portCount + 2);

		// Command prefix (if any)
		if (Command != FrameCommand.None) {
			parts.Add(GetCommandPrefix());
		}

		// Controller inputs
		for (int i = 0; i < portCount && i < Controllers.Length; i++) {
			parts.Add(Controllers[i].ToNexenFormat());
		}

		// Lag frame marker
		if (IsLagFrame) {
			parts.Add("LAG");
		}

		// Comment (if any)
		if (!string.IsNullOrEmpty(Comment)) {
			parts.Add($"# {Comment}");
		}

		return string.Join("|", parts);
	}

	/// <summary>
	/// Parse from Nexen input log format
	/// </summary>
	/// <param name="line">Single line from input log</param>
	/// <param name="frameNumber">Frame number to assign</param>
	/// <returns>Parsed InputFrame</returns>
	public static InputFrame FromNexenLogLine(string line, int frameNumber) {
		var frame = new InputFrame(frameNumber);

		// Handle empty lines
		if (string.IsNullOrWhiteSpace(line)) {
			return frame;
		}

		// Split by pipe
		var parts = line.Split('|');
		int portIndex = 0;

		foreach (var part in parts) {
			var trimmed = part.Trim();

			// Skip empty parts
			if (string.IsNullOrEmpty(trimmed)) {
				continue;
			}

			// Check for special markers
			if (trimmed.StartsWith('#')) {
				// Comment
				frame.Comment = trimmed[1..].Trim();
				continue;
			}

			if (trimmed == "LAG") {
				frame.IsLagFrame = true;
				continue;
			}

			if (trimmed.StartsWith("CMD:")) {
				// Command
				frame.Command = ParseCommand(trimmed[4..]);
				continue;
			}

			// Controller input
			if (portIndex < MaxPorts && trimmed.Length >= 8) {
				frame.Controllers[portIndex] = ControllerInput.FromNexenFormat(trimmed);
				portIndex++;
			}
		}

		return frame;
	}

	/// <summary>
	/// Create a deep copy of this frame
	/// </summary>
	public InputFrame Clone() {
		var clone = new InputFrame(FrameNumber) {
			Command = Command,
			IsLagFrame = IsLagFrame,
			Comment = Comment
		};

		for (int i = 0; i < Controllers.Length; i++) {
			clone.Controllers[i] = Controllers[i].Clone();
		}

		return clone;
	}

	private string GetCommandPrefix() {
		var commands = new List<string>();

		if (Command.HasFlag(FrameCommand.SoftReset)) commands.Add("SOFT_RESET");
		if (Command.HasFlag(FrameCommand.HardReset)) commands.Add("HARD_RESET");
		if (Command.HasFlag(FrameCommand.FdsInsert)) commands.Add("FDS_INSERT");
		if (Command.HasFlag(FrameCommand.FdsSelect)) commands.Add("FDS_SELECT");
		if (Command.HasFlag(FrameCommand.VsInsertCoin)) commands.Add("VS_COIN");
		if (Command.HasFlag(FrameCommand.ControllerSwap)) commands.Add("CTRL_SWAP");

		return $"CMD:{string.Join(',', commands)}";
	}

	private static FrameCommand ParseCommand(string commandStr) {
		var command = FrameCommand.None;
		var parts = commandStr.Split(',');

		foreach (var part in parts) {
			command |= part.Trim().ToUpperInvariant() switch {
				"SOFT_RESET" => FrameCommand.SoftReset,
				"HARD_RESET" => FrameCommand.HardReset,
				"FDS_INSERT" => FrameCommand.FdsInsert,
				"FDS_SELECT" => FrameCommand.FdsSelect,
				"VS_COIN" => FrameCommand.VsInsertCoin,
				"CTRL_SWAP" => FrameCommand.ControllerSwap,
				_ => FrameCommand.None
			};
		}

		return command;
	}

	public override string ToString() => $"Frame {FrameNumber}: {ToNexenLogLine()}";
}
