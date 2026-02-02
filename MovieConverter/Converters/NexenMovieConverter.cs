using System.IO.Compression;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Nexen.MovieConverter.Converters;

/// <summary>
/// Converter for Nexen native movie format (.nexen-movie).
/// 
/// The .nexen-movie format is a ZIP archive containing:
/// - movie.json: Metadata (author, ROM info, settings)
/// - input.txt: Human-readable input log (one frame per line)
/// - savestate.bin: Optional initial savestate
/// - sram.bin: Optional initial SRAM
/// </summary>
public class NexenMovieConverter : MovieConverterBase {
	/// <inheritdoc/>
	public override string[] Extensions => [".nexen-movie"];

	/// <inheritdoc/>
	public override string FormatName => "Nexen Movie";

	/// <inheritdoc/>
	public override MovieFormat Format => MovieFormat.Nexen;

	private static readonly JsonSerializerOptions JsonOptions = new() {
		WriteIndented = true,
		PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
		DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
		Converters = { new JsonStringEnumConverter(JsonNamingPolicy.CamelCase) }
	};

	/// <inheritdoc/>
	public override MovieData Read(Stream stream, string? fileName = null) {
		using var archive = new ZipArchive(stream, ZipArchiveMode.Read, leaveOpen: true);

		// Read metadata
		var metadataEntry = archive.GetEntry("movie.json")
			?? throw new MovieFormatException(Format, "Missing movie.json in archive");

		MovieMetadata metadata;
		using (var metadataStream = metadataEntry.Open()) {
			metadata = JsonSerializer.Deserialize<MovieMetadata>(metadataStream, JsonOptions)
				?? throw new MovieFormatException(Format, "Invalid movie.json");
		}

		// Create movie from metadata
		var movie = new MovieData {
			Author = metadata.Author ?? "",
			Description = metadata.Description ?? "",
			GameName = metadata.GameName ?? "",
			RomFileName = metadata.RomFileName ?? "",
			Sha1Hash = metadata.Sha1Hash,
			Sha256Hash = metadata.Sha256Hash,
			Md5Hash = metadata.Md5Hash,
			Crc32 = metadata.Crc32,
			SystemType = metadata.SystemType,
			Region = metadata.Region,
			RerecordCount = metadata.RerecordCount,
			LagFrameCount = metadata.LagFrameCount,
			ControllerCount = metadata.ControllerCount,
			SourceFormat = MovieFormat.Nexen,
			SourceFormatVersion = metadata.FormatVersion,
			EmulatorVersion = metadata.EmulatorVersion,
			CreatedDate = metadata.CreatedDate
		};

		// Copy port types
		if (metadata.PortTypes != null) {
			for (int i = 0; i < metadata.PortTypes.Length && i < movie.PortTypes.Length; i++) {
				movie.PortTypes[i] = metadata.PortTypes[i];
			}
		}

		// Read input log
		var inputEntry = archive.GetEntry("input.txt");
		if (inputEntry != null) {
			using var inputStream = inputEntry.Open();
			using var reader = new StreamReader(inputStream);

			int frameNumber = 0;
			while (reader.ReadLine() is { } line) {
				// Skip empty lines and comments
				if (string.IsNullOrWhiteSpace(line) || line.TrimStart().StartsWith("//")) {
					continue;
				}

				var frame = InputFrame.FromNexenLogLine(line, frameNumber);
				movie.InputFrames.Add(frame);
				frameNumber++;
			}
		}

		// Read optional savestate
		var savestateEntry = archive.GetEntry("savestate.bin");
		if (savestateEntry != null) {
			using var ssStream = savestateEntry.Open();
			movie.InitialState = ReadAllBytes(ssStream);
		}

		// Read optional SRAM
		var sramEntry = archive.GetEntry("sram.bin");
		if (sramEntry != null) {
			using var sramStream = sramEntry.Open();
			movie.InitialSram = ReadAllBytes(sramStream);
		}

		return movie;
	}

	/// <inheritdoc/>
	public override void Write(MovieData movie, Stream stream) {
		using var archive = new ZipArchive(stream, ZipArchiveMode.Create, leaveOpen: true);

		// Write metadata
		var metadata = new MovieMetadata {
			FormatVersion = "1.0",
			EmulatorVersion = movie.EmulatorVersion ?? "Nexen",
			CreatedDate = movie.CreatedDate ?? DateTime.UtcNow,

			Author = movie.Author,
			Description = movie.Description,
			GameName = movie.GameName,
			RomFileName = movie.RomFileName,

			Sha1Hash = movie.Sha1Hash,
			Sha256Hash = movie.Sha256Hash,
			Md5Hash = movie.Md5Hash,
			Crc32 = movie.Crc32,

			SystemType = movie.SystemType,
			Region = movie.Region,
			ControllerCount = movie.ControllerCount,
			PortTypes = movie.PortTypes,

			RerecordCount = movie.RerecordCount,
			LagFrameCount = movie.LagFrameCount,
			TotalFrames = movie.TotalFrames,

			StartsFromSavestate = movie.StartsFromSavestate,
			StartsFromSram = movie.StartsFromSram
		};

		var metadataEntry = archive.CreateEntry("movie.json", CompressionLevel.Optimal);
		using (var metadataStream = metadataEntry.Open()) {
			JsonSerializer.Serialize(metadataStream, metadata, JsonOptions);
		}

		// Write input log
		var inputEntry = archive.CreateEntry("input.txt", CompressionLevel.Optimal);
		using (var inputStream = inputEntry.Open())
		using (var writer = new StreamWriter(inputStream, Encoding.UTF8)) {
			// Header comment
			writer.WriteLine($"// Nexen Movie Input Log");
			writer.WriteLine($"// Game: {movie.GameName}");
			writer.WriteLine($"// Author: {movie.Author}");
			writer.WriteLine($"// Frames: {movie.TotalFrames}");
			writer.WriteLine($"// Format: P1|P2|...|[LAG]|[# comment]");
			writer.WriteLine($"// Buttons: BYsSUDLRAXLR (SNES) or ........UDLRSTBA (NES)");
			writer.WriteLine();

			foreach (var frame in movie.InputFrames) {
				writer.WriteLine(frame.ToNexenLogLine(movie.ControllerCount));
			}
		}

		// Write optional savestate
		if (movie.InitialState != null && movie.InitialState.Length > 0) {
			var ssEntry = archive.CreateEntry("savestate.bin", CompressionLevel.Optimal);
			using var ssStream = ssEntry.Open();
			ssStream.Write(movie.InitialState);
		}

		// Write optional SRAM
		if (movie.InitialSram != null && movie.InitialSram.Length > 0) {
			var sramEntry = archive.CreateEntry("sram.bin", CompressionLevel.Optimal);
			using var sramStream = sramEntry.Open();
			sramStream.Write(movie.InitialSram);
		}
	}

	/// <summary>
	/// Internal metadata structure for JSON serialization
	/// </summary>
	private class MovieMetadata {
		// Format info
		public string? FormatVersion { get; set; }
		public string? EmulatorVersion { get; set; }
		public DateTime? CreatedDate { get; set; }

		// Movie info
		public string? Author { get; set; }
		public string? Description { get; set; }
		public string? GameName { get; set; }
		public string? RomFileName { get; set; }

		// ROM hashes
		public string? Sha1Hash { get; set; }
		public string? Sha256Hash { get; set; }
		public string? Md5Hash { get; set; }
		public uint? Crc32 { get; set; }

		// System
		public SystemType SystemType { get; set; }
		public RegionType Region { get; set; }
		public int ControllerCount { get; set; }
		public ControllerType[]? PortTypes { get; set; }

		// Statistics
		public uint RerecordCount { get; set; }
		public uint LagFrameCount { get; set; }
		public int TotalFrames { get; set; }

		// Start state
		public bool StartsFromSavestate { get; set; }
		public bool StartsFromSram { get; set; }
	}
}
