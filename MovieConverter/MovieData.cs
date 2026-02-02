using System.Text;

namespace Nexen.MovieConverter;

/// <summary>
/// Common movie data structure that can be converted between TAS formats.
/// Contains all metadata and input frames for a TAS movie.
/// </summary>
public class MovieData {
	// ========== Metadata ==========

	/// <summary>Author(s) of the TAS</summary>
	public string Author { get; set; } = "";

	/// <summary>Description or comments about the TAS</summary>
	public string Description { get; set; } = "";

	/// <summary>Name of the game</summary>
	public string GameName { get; set; } = "";

	/// <summary>ROM filename (without path)</summary>
	public string RomFileName { get; set; } = "";

	// ========== ROM Identification ==========

	/// <summary>SHA-1 hash of the ROM (40 hex chars)</summary>
	public string? Sha1Hash { get; set; }

	/// <summary>SHA-256 hash of the ROM (64 hex chars)</summary>
	public string? Sha256Hash { get; set; }

	/// <summary>MD5 hash of the ROM (32 hex chars)</summary>
	public string? Md5Hash { get; set; }

	/// <summary>CRC32 hash of the ROM</summary>
	public uint? Crc32 { get; set; }

	// ========== System/Timing ==========

	/// <summary>Target console/system</summary>
	public SystemType SystemType { get; set; } = SystemType.Snes;

	/// <summary>Video region (NTSC/PAL)</summary>
	public RegionType Region { get; set; } = RegionType.NTSC;

	/// <summary>Frame rate based on region</summary>
	public double FrameRate => Region switch {
		RegionType.PAL => SystemType switch {
			SystemType.Nes => 50.006978,
			SystemType.Snes => 50.006979,
			SystemType.Gb or SystemType.Gbc => 59.7275, // GB is always ~60Hz
			_ => 50.0
		},
		_ => SystemType switch {
			SystemType.Nes => 60.098814,
			SystemType.Snes => 60.098814,
			SystemType.Gb or SystemType.Gbc => 59.7275,
			SystemType.Gba => 59.7275,
			_ => 60.0
		}
	};

	// ========== Movie Statistics ==========

	/// <summary>Number of times the movie was re-recorded (savestates loaded during recording)</summary>
	public uint RerecordCount { get; set; }

	/// <summary>Number of lag frames (frames where input wasn't polled)</summary>
	public uint LagFrameCount { get; set; }

	/// <summary>Total number of input frames</summary>
	public int TotalFrames => InputFrames.Count;

	/// <summary>Movie duration based on frame count and rate</summary>
	public TimeSpan Duration => TimeSpan.FromSeconds(TotalFrames / FrameRate);

	// ========== Controller Configuration ==========

	/// <summary>Number of active controller ports</summary>
	public int ControllerCount { get; set; } = 2;

	/// <summary>Controller type for each port</summary>
	public ControllerType[] PortTypes { get; set; } = new ControllerType[InputFrame.MaxPorts];

	// ========== Movie Data ==========

	/// <summary>All input frames in the movie</summary>
	public List<InputFrame> InputFrames { get; set; } = [];

	// ========== Save State / SRAM ==========

	/// <summary>Initial savestate data (for movies starting from savestate)</summary>
	public byte[]? InitialState { get; set; }

	/// <summary>Initial SRAM data (for movies starting from SRAM)</summary>
	public byte[]? InitialSram { get; set; }

	/// <summary>Whether the movie starts from a savestate</summary>
	public bool StartsFromSavestate => InitialState != null && InitialState.Length > 0;

	/// <summary>Whether the movie starts from SRAM</summary>
	public bool StartsFromSram => InitialSram != null && InitialSram.Length > 0;

	/// <summary>Whether the movie starts from a clean power-on state</summary>
	public bool StartsFromPowerOn => !StartsFromSavestate && !StartsFromSram;

	// ========== Source Format Info ==========

	/// <summary>Original format this movie was loaded from</summary>
	public MovieFormat SourceFormat { get; set; }

	/// <summary>Version of the source format</summary>
	public string? SourceFormatVersion { get; set; }

	/// <summary>Emulator that created the movie</summary>
	public string? EmulatorVersion { get; set; }

	/// <summary>Date the movie was created</summary>
	public DateTime? CreatedDate { get; set; }

	// ========== Constructor ==========

	public MovieData() {
		// Default: first two ports are gamepads, rest are disconnected
		for (int i = 0; i < PortTypes.Length; i++) {
			PortTypes[i] = i < 2 ? ControllerType.Gamepad : ControllerType.None;
		}
	}

	// ========== Methods ==========

	/// <summary>
	/// Add an input frame to the movie
	/// </summary>
	public void AddFrame(InputFrame frame) {
		frame.FrameNumber = InputFrames.Count;
		InputFrames.Add(frame);
	}

	/// <summary>
	/// Get frame at the specified index
	/// </summary>
	/// <param name="index">0-based frame index</param>
	/// <returns>The input frame, or null if out of range</returns>
	public InputFrame? GetFrame(int index) {
		if (index < 0 || index >= InputFrames.Count) {
			return null;
		}
		return InputFrames[index];
	}

	/// <summary>
	/// Truncate movie to the specified frame (for rerecording)
	/// </summary>
	/// <param name="frameNumber">Last frame to keep (inclusive)</param>
	public void TruncateAt(int frameNumber) {
		if (frameNumber < 0) {
			InputFrames.Clear();
			return;
		}

		if (frameNumber < InputFrames.Count) {
			InputFrames.RemoveRange(frameNumber + 1, InputFrames.Count - frameNumber - 1);
		}
	}

	/// <summary>
	/// Calculate the CRC32 of the input data (for verification)
	/// </summary>
	public uint CalculateInputCrc32() {
		// Simple CRC32 of all input data
		var data = new List<byte>();
		foreach (var frame in InputFrames) {
			for (int i = 0; i < ControllerCount; i++) {
				var smv = frame.Controllers[i].ToSmvFormat();
				data.Add((byte)(smv & 0xff));
				data.Add((byte)(smv >> 8));
			}
		}
		return ComputeCrc32(data.ToArray());
	}

	/// <summary>
	/// Simple CRC32 implementation
	/// </summary>
	private static uint ComputeCrc32(byte[] data) {
		uint crc = 0xffffffff;
		foreach (var b in data) {
			crc ^= b;
			for (int i = 0; i < 8; i++) {
				crc = (crc >> 1) ^ (0xedb88320 * (crc & 1));
			}
		}
		return ~crc;
	}

	/// <summary>
	/// Get a human-readable summary of the movie
	/// </summary>
	public string GetSummary() {
		var sb = new StringBuilder();

		sb.AppendLine("Movie Summary");
		sb.AppendLine("═════════════════════════════════════════");
		sb.AppendLine();

		// Source Info
		sb.AppendLine($"Source Format:   {SourceFormat} {SourceFormatVersion}");
		if (!string.IsNullOrEmpty(EmulatorVersion)) {
			sb.AppendLine($"Emulator:        {EmulatorVersion}");
		}
		sb.AppendLine();

		// Game Info
		sb.AppendLine($"System:          {SystemType} ({Region})");
		if (!string.IsNullOrEmpty(GameName)) {
			sb.AppendLine($"Game:            {GameName}");
		}
		if (!string.IsNullOrEmpty(RomFileName)) {
			sb.AppendLine($"ROM:             {RomFileName}");
		}
		if (!string.IsNullOrEmpty(Author)) {
			sb.AppendLine($"Author:          {Author}");
		}
		sb.AppendLine();

		// Statistics
		sb.AppendLine("Statistics");
		sb.AppendLine("───────────────────────────────────────────");
		sb.AppendLine($"Total Frames:    {TotalFrames:N0}");
		sb.AppendLine($"Duration:        {Duration:hh\\:mm\\:ss\\.fff}");
		sb.AppendLine($"Rerecords:       {RerecordCount:N0}");
		sb.AppendLine($"Lag Frames:      {LagFrameCount:N0}");
		sb.AppendLine();

		// Start State
		sb.AppendLine("Start State");
		sb.AppendLine("───────────────────────────────────────────");
		sb.AppendLine($"From Savestate:  {StartsFromSavestate}");
		sb.AppendLine($"From SRAM:       {StartsFromSram}");
		sb.AppendLine($"Clean Start:     {StartsFromPowerOn}");
		sb.AppendLine();

		// Controllers
		sb.AppendLine($"Controllers:     {ControllerCount}");
		for (int i = 0; i < ControllerCount; i++) {
			sb.AppendLine($"  Port {i + 1}:        {PortTypes[i]}");
		}

		// ROM Hashes
		if (Crc32.HasValue || !string.IsNullOrEmpty(Sha1Hash) || !string.IsNullOrEmpty(Md5Hash)) {
			sb.AppendLine();
			sb.AppendLine("ROM Identification");
			sb.AppendLine("───────────────────────────────────────────");
			if (Crc32.HasValue) {
				sb.AppendLine($"CRC32:           {Crc32:x8}");
			}
			if (!string.IsNullOrEmpty(Md5Hash)) {
				sb.AppendLine($"MD5:             {Md5Hash.ToLowerInvariant()}");
			}
			if (!string.IsNullOrEmpty(Sha1Hash)) {
				sb.AppendLine($"SHA-1:           {Sha1Hash.ToLowerInvariant()}");
			}
		}

		return sb.ToString();
	}

	/// <summary>
	/// Create a deep copy of this movie
	/// </summary>
	public MovieData Clone() {
		var clone = new MovieData {
			Author = Author,
			Description = Description,
			GameName = GameName,
			RomFileName = RomFileName,
			Sha1Hash = Sha1Hash,
			Sha256Hash = Sha256Hash,
			Md5Hash = Md5Hash,
			Crc32 = Crc32,
			SystemType = SystemType,
			Region = Region,
			RerecordCount = RerecordCount,
			LagFrameCount = LagFrameCount,
			ControllerCount = ControllerCount,
			SourceFormat = SourceFormat,
			SourceFormatVersion = SourceFormatVersion,
			EmulatorVersion = EmulatorVersion,
			CreatedDate = CreatedDate
		};

		// Clone port types
		Array.Copy(PortTypes, clone.PortTypes, PortTypes.Length);

		// Clone frames
		clone.InputFrames = new List<InputFrame>(InputFrames.Count);
		foreach (var frame in InputFrames) {
			clone.InputFrames.Add(frame.Clone());
		}

		// Clone state data
		if (InitialState != null) {
			clone.InitialState = new byte[InitialState.Length];
			Array.Copy(InitialState, clone.InitialState, InitialState.Length);
		}

		if (InitialSram != null) {
			clone.InitialSram = new byte[InitialSram.Length];
			Array.Copy(InitialSram, clone.InitialSram, InitialSram.Length);
		}

		return clone;
	}

	public override string ToString() =>
		$"{GameName} - {TotalFrames} frames ({Duration:mm\\:ss\\.ff}) by {Author}";
}
