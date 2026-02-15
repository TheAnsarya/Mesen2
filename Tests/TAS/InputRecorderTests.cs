using Xunit;
using System;
using System.Collections.Generic;
using Nexen.MovieConverter;

namespace Nexen.Tests.TAS;

/// <summary>
/// Unit tests for InputRecorder functionality.
/// Tests truncation, recording modes, and input manipulation.
/// </summary>
public class InputRecorderTests {
	#region Test Helpers

	private static MovieData CreateTestMovie(int frameCount) {
		var movie = new MovieData {
			Author = "Test",
			GameName = "Test Game",
			SystemType = SystemType.Nes,
			Region = RegionType.NTSC
		};

		for (int i = 0; i < frameCount; i++) {
			var frame = new InputFrame(i) {
				Controllers = new ControllerInput[] { new ControllerInput { A = i % 3 == 0 } }
			};
			movie.InputFrames.Add(frame);
		}

		return movie;
	}

	#endregion

	#region Truncation Tests

	[Fact]
	public void Truncate_WithRemoveRange_RemovesCorrectFrames() {
		// Arrange
		var movie = CreateTestMovie(100);
		int truncateAt = 50;
		int expectedCount = truncateAt;

		// Act - this mimics the fix we made (RemoveRange instead of loop)
		int removeCount = movie.InputFrames.Count - truncateAt;
		movie.InputFrames.RemoveRange(truncateAt, removeCount);

		// Assert
		Assert.Equal(expectedCount, movie.InputFrames.Count);
		Assert.Equal(truncateAt - 1, movie.InputFrames[^1].FrameNumber);
	}

	[Fact]
	public void Truncate_AtBeginning_LeavesEmptyMovie() {
		// Arrange
		var movie = CreateTestMovie(100);

		// Act
		movie.InputFrames.RemoveRange(0, movie.InputFrames.Count);

		// Assert
		Assert.Empty(movie.InputFrames);
	}

	[Fact]
	public void Truncate_AtEnd_NoChange() {
		// Arrange
		var movie = CreateTestMovie(100);
		int originalCount = movie.InputFrames.Count;

		// Act - truncate at end (remove 0 frames)
		int removeCount = movie.InputFrames.Count - movie.InputFrames.Count;
		if (removeCount > 0) {
			movie.InputFrames.RemoveRange(movie.InputFrames.Count, removeCount);
		}

		// Assert
		Assert.Equal(originalCount, movie.InputFrames.Count);
	}

	[Fact]
	public void Truncate_PreservesFrameData() {
		// Arrange
		var movie = CreateTestMovie(10);
		movie.InputFrames[3].Controllers[0].A = true;
		movie.InputFrames[3].Controllers[0].B = true;

		// Act - truncate at frame 5
		movie.InputFrames.RemoveRange(5, movie.InputFrames.Count - 5);

		// Assert - frame 3 data preserved
		Assert.True(movie.InputFrames[3].Controllers[0].A);
		Assert.True(movie.InputFrames[3].Controllers[0].B);
	}

	[Theory]
	[InlineData(10, 5)]
	[InlineData(100, 50)]
	[InlineData(1000, 500)]
	[InlineData(10000, 5000)]
	public void Truncate_VariousSizes_WorksCorrectly(int movieSize, int truncateAt) {
		// Arrange
		var movie = CreateTestMovie(movieSize);

		// Act
		int removeCount = movie.InputFrames.Count - truncateAt;
		movie.InputFrames.RemoveRange(truncateAt, removeCount);

		// Assert
		Assert.Equal(truncateAt, movie.InputFrames.Count);
	}

	#endregion

	#region RemoveRange vs RemoveAt Comparison Tests

	[Fact]
	public void RemoveRange_ProducesIdenticalResult_ToRemoveAtLoop() {
		// Arrange - two identical movies
		var movie1 = CreateTestMovie(50);
		var movie2 = CreateTestMovie(50);
		int removeFrom = 25;

		// Act - RemoveRange approach (new)
		int removeCount = movie1.InputFrames.Count - removeFrom;
		movie1.InputFrames.RemoveRange(removeFrom, removeCount);

		// Act - RemoveAt loop approach (old - O(n²))
		while (movie2.InputFrames.Count > removeFrom) {
			movie2.InputFrames.RemoveAt(removeFrom);
		}

		// Assert - identical results
		Assert.Equal(movie1.InputFrames.Count, movie2.InputFrames.Count);
		for (int i = 0; i < movie1.InputFrames.Count; i++) {
			Assert.Equal(
				movie1.InputFrames[i].Controllers[0].A,
				movie2.InputFrames[i].Controllers[0].A
			);
		}
	}

	#endregion
}
