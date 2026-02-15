using BenchmarkDotNet.Attributes;
using BenchmarkDotNet.Configs;
using BenchmarkDotNet.Order;
using Nexen.MovieConverter;
using Nexen.MovieConverter.Converters;

namespace Nexen.Benchmarks;

/// <summary>
/// Benchmarks for TAS/Movie hot paths.
/// Focuses on operations that happen every frame or frequently during TAS editing.
/// </summary>
[MemoryDiagnoser]
[Orderer(SummaryOrderPolicy.FastestToSlowest)]
[GroupBenchmarksBy(BenchmarkLogicalGroupRule.ByCategory)]
public class TasHotPathBenchmarks {
	// Test data
	private MovieData _smallMovie = null!;   // 1,000 frames
	private MovieData _mediumMovie = null!;  // 60,000 frames (~16 min at 60fps)
	private MovieData _largeMovie = null!;   // 300,000 frames (~83 min at 60fps)

	private ControllerInput[] _testInputs = null!;
	private InputFrame _testFrame = null!;

	[GlobalSetup]
	public void Setup() {
		var random = new Random(42); // Deterministic seed

		_smallMovie = GenerateMovie(1_000, random);
		_mediumMovie = GenerateMovie(60_000, random);
		_largeMovie = GenerateMovie(300_000, random);

		_testInputs = new ControllerInput[4];
		for (int i = 0; i < 4; i++) {
			_testInputs[i] = GenerateRandomInput(random);
		}

		_testFrame = new InputFrame(0);
		for (int i = 0; i < _testFrame.Controllers.Length; i++) {
			_testFrame.Controllers[i] = GenerateRandomInput(random);
		}
	}

	private static MovieData GenerateMovie(int frameCount, Random random) {
		var movie = new MovieData {
			Author = "Benchmark Test",
			GameName = "Test Game",
			SystemType = SystemType.Snes,
			Region = RegionType.NTSC
		};

		for (int i = 0; i < frameCount; i++) {
			var frame = new InputFrame(i) {
				IsLagFrame = random.NextDouble() < 0.05 // 5% lag frames
			};

			// Generate realistic button patterns
			for (int c = 0; c < frame.Controllers.Length; c++) {
				frame.Controllers[c] = GenerateRandomInput(random);
			}

			movie.InputFrames.Add(frame);
		}

		return movie;
	}

	private static ControllerInput GenerateRandomInput(Random random) {
		return new ControllerInput {
			A = random.NextDouble() < 0.1,
			B = random.NextDouble() < 0.1,
			X = random.NextDouble() < 0.05,
			Y = random.NextDouble() < 0.05,
			L = random.NextDouble() < 0.02,
			R = random.NextDouble() < 0.02,
			Start = random.NextDouble() < 0.001,
			Select = random.NextDouble() < 0.001,
			Up = random.NextDouble() < 0.15,
			Down = random.NextDouble() < 0.15,
			Left = random.NextDouble() < 0.1,
			Right = random.NextDouble() < 0.2
		};
	}

	#region Input Comparison Benchmarks

	/// <summary>
	/// Benchmark comparing button bits - used for input mismatch detection
	/// </summary>
	[Benchmark(Description = "ButtonBits comparison")]
	[BenchmarkCategory("Input")]
	public bool ButtonBitsComparison() {
		return _testInputs[0].ButtonBits == _testInputs[1].ButtonBits;
	}

	/// <summary>
	/// Benchmark for checking if any button is pressed
	/// </summary>
	[Benchmark(Description = "Any button pressed check")]
	[BenchmarkCategory("Input")]
	public bool AnyButtonPressed() {
		return _testInputs[0].ButtonBits != 0;
	}

	/// <summary>
	/// Benchmark input mismatch check for all controllers
	/// </summary>
	[Benchmark(Description = "Input mismatch detection (4 controllers)")]
	[BenchmarkCategory("Input")]
	public bool InputMismatchCheck() {
		var frameInput = _smallMovie.InputFrames[100];
		bool mismatch = false;
		for (int i = 0; i < _testInputs.Length && i < frameInput.Controllers.Length; i++) {
			if (_testInputs[i].ButtonBits != 0 &&
			    _testInputs[i].ButtonBits != frameInput.Controllers[i].ButtonBits) {
				mismatch = true;
				break;
			}
		}
		return mismatch;
	}

	#endregion

	#region Frame Operations

	/// <summary>
	/// Benchmark adding a frame to the movie
	/// </summary>
	[Benchmark(Description = "Add frame to movie")]
	[BenchmarkCategory("FrameOps")]
	public void AddFrame() {
		_smallMovie.InputFrames.Add(_testFrame);
		_smallMovie.InputFrames.RemoveAt(_smallMovie.InputFrames.Count - 1); // Clean up
	}

	/// <summary>
	/// Benchmark inserting a frame at a specific position
	/// </summary>
	[Benchmark(Description = "Insert frame at position 500")]
	[BenchmarkCategory("FrameOps")]
	public void InsertFrame() {
		_smallMovie.InputFrames.Insert(500, _testFrame);
		_smallMovie.InputFrames.RemoveAt(500); // Clean up
	}

	/// <summary>
	/// Benchmark getting frame by index
	/// </summary>
	[Benchmark(Description = "Get frame by index")]
	[BenchmarkCategory("FrameOps")]
	public InputFrame GetFrame() {
		return _mediumMovie.InputFrames[30000];
	}

	#endregion

	#region Serialization Benchmarks

	/// <summary>
	/// Benchmark serializing movie to Nexen format
	/// </summary>
	[Benchmark(Description = "Serialize 1K frames to Nexen")]
	[BenchmarkCategory("Serialize")]
	public void SerializeSmallMovie() {
		var converter = new NexenMovieConverter();
		using var stream = new MemoryStream();
		converter.Write(_smallMovie, stream);
	}

	/// <summary>
	/// Benchmark round-trip for small movie
	/// </summary>
	[Benchmark(Description = "Round-trip 1K frames")]
	[BenchmarkCategory("Serialize")]
	public MovieData RoundTripSmallMovie() {
		var converter = new NexenMovieConverter();
		using var stream = new MemoryStream();
		converter.Write(_smallMovie, stream);
		stream.Position = 0;
		return converter.Read(stream);
	}

	#endregion

	#region Controller Input Creation

	/// <summary>
	/// Benchmark creating a new InputFrame
	/// </summary>
	[Benchmark(Description = "Create InputFrame")]
	[BenchmarkCategory("Creation")]
	public InputFrame CreateInputFrame() {
		return new InputFrame(0);
	}

	/// <summary>
	/// Benchmark creating a new ControllerInput
	/// </summary>
	[Benchmark(Description = "Create ControllerInput")]
	[BenchmarkCategory("Creation")]
	public ControllerInput CreateControllerInput() {
		return new ControllerInput {
			A = true,
			B = false,
			Right = true,
			Up = false
		};
	}

	#endregion

	#region Lag Frame Detection

	/// <summary>
	/// Benchmark counting lag frames in small movie
	/// </summary>
	[Benchmark(Description = "Count lag frames (1K)")]
	[BenchmarkCategory("LagFrames")]
	public int CountLagFramesSmall() {
		int count = 0;
		foreach (var frame in _smallMovie.InputFrames) {
			if (frame.IsLagFrame) count++;
		}
		return count;
	}

	/// <summary>
	/// Benchmark counting lag frames in medium movie using LINQ
	/// </summary>
	[Benchmark(Description = "Count lag frames (60K) LINQ")]
	[BenchmarkCategory("LagFrames")]
	public int CountLagFramesMediumLinq() {
		return _mediumMovie.InputFrames.Count(f => f.IsLagFrame);
	}

	/// <summary>
	/// Benchmark counting lag frames in medium movie using foreach
	/// </summary>
	[Benchmark(Description = "Count lag frames (60K) foreach")]
	[BenchmarkCategory("LagFrames")]
	public int CountLagFramesMediumForeach() {
		int count = 0;
		foreach (var frame in _mediumMovie.InputFrames) {
			if (frame.IsLagFrame) count++;
		}
		return count;
	}

	#endregion

	#region Movie Lookup Operations

	/// <summary>
	/// Benchmark finding frames with specific input pattern
	/// </summary>
	[Benchmark(Description = "Find frames with A+B (1K)")]
	[BenchmarkCategory("Lookup")]
	public List<int> FindFramesWithPattern() {
		var results = new List<int>();
		for (int i = 0; i < _smallMovie.InputFrames.Count; i++) {
			var input = _smallMovie.InputFrames[i].Controllers[0];
			if (input.A && input.B) {
				results.Add(i);
			}
		}
		return results;
	}

	#endregion
}
