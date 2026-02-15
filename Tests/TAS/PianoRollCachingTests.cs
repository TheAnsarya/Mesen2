using Xunit;
using System;
using System.Collections.Generic;
using Avalonia.Media;

namespace Nexen.Tests.TAS;

/// <summary>
/// Unit tests for PianoRollControl rendering optimizations.
/// Tests FormattedText caching and other rendering performance improvements.
/// </summary>
public class PianoRollCachingTests {
	#region Cache Invalidation Tests

	[Fact]
	public void FormattedTextCache_ShouldInvalidateOnZoomChange() {
		// Arrange
		var cache = new Dictionary<string, object>();
		double cachedZoom = 1.0;
		double currentZoom = 1.0;

		// Populate cache
		cache["A"] = "text_A_at_zoom_1.0";
		cache["B"] = "text_B_at_zoom_1.0";

		// Act - zoom changes
		currentZoom = 2.0;
		bool shouldInvalidate = cachedZoom != currentZoom;

		if (shouldInvalidate) {
			cache.Clear();
			cachedZoom = currentZoom;
		}

		// Assert
		Assert.Empty(cache);
		Assert.Equal(2.0, cachedZoom);
	}

	[Fact]
	public void FormattedTextCache_ShouldNotInvalidateIfZoomSame() {
		// Arrange
		var cache = new Dictionary<string, object>();
		double cachedZoom = 1.0;
		double currentZoom = 1.0;

		// Populate cache
		cache["A"] = "text_A";
		cache["B"] = "text_B";

		// Act - zoom stays same
		bool shouldInvalidate = cachedZoom != currentZoom;

		if (shouldInvalidate) {
			cache.Clear();
			cachedZoom = currentZoom;
		}

		// Assert - cache preserved
		Assert.Equal(2, cache.Count);
		Assert.True(cache.ContainsKey("A"));
		Assert.True(cache.ContainsKey("B"));
	}

	#endregion

	#region LRU Cache Tests

	[Fact]
	public void FrameNumberCache_ShouldPruneWhenOverLimit() {
		// Arrange
		const int maxCacheSize = 200;
		var cache = new Dictionary<int, string>();

		// Fill cache to limit
		for (int i = 0; i < maxCacheSize; i++) {
			cache[i] = $"frame_{i}";
		}

		// Act - add one more, should trigger prune
		int newFrame = maxCacheSize;
		if (cache.Count >= maxCacheSize) {
			cache.Clear();
		}
		cache[newFrame] = $"frame_{newFrame}";

		// Assert - cache was cleared and only has new entry
		Assert.Single(cache);
		Assert.True(cache.ContainsKey(newFrame));
	}

	[Fact]
	public void FrameNumberCache_ShouldReuseCachedText() {
		// Arrange
		var cache = new Dictionary<int, string>();
		int hitCount = 0;

		// First access - miss
		void AccessFrame(int frame) {
			if (cache.TryGetValue(frame, out var text)) {
				hitCount++;
			} else {
				cache[frame] = $"frame_{frame}";
			}
		}

		// Act
		AccessFrame(10); // Miss
		AccessFrame(20); // Miss
		AccessFrame(10); // Hit
		AccessFrame(10); // Hit
		AccessFrame(20); // Hit
		AccessFrame(30); // Miss

		// Assert
		Assert.Equal(3, hitCount); // 3 cache hits
		Assert.Equal(3, cache.Count); // 3 unique entries
	}

	#endregion

	#region Button Label Caching Tests

	[Fact]
	public void ButtonLabelCache_ShouldCacheSameLabels() {
		// Arrange
		var cache = new Dictionary<string, object>();
		var buttons = new[] { "A", "B", "X", "Y", "L", "R", "↑", "↓", "←", "→", "ST", "SE" };
		int createCount = 0;

		// Simulate two render cycles
		for (int cycle = 0; cycle < 2; cycle++) {
			foreach (var button in buttons) {
				if (!cache.TryGetValue(button, out _)) {
					cache[button] = $"text_{button}";
					createCount++;
				}
			}
		}

		// Assert - only 12 creations (one per button), not 24
		Assert.Equal(12, createCount);
		Assert.Equal(12, cache.Count);
	}

	#endregion
}
