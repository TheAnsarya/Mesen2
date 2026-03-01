using System;
using System.Threading;
using System.Threading.Tasks;
using Avalonia.Threading;
using Nexen.Config;
using Nexen.Debugger.Labels;
using Nexen.Interop;
using Nexen.Utilities;

namespace Nexen.Debugger.Labels;
/// <summary>
/// Manages background CDL recording and periodic Pansy file export.
/// This enables CDL collection without requiring the debugger window to be open.
/// </summary>
public static class BackgroundPansyExporter {
	private static Timer? _autoSaveTimer;
	private static RomInfo? _currentRomInfo;
	private static bool _cdlEnabled;
	/// <summary>Cached ROM CRC32 — computed once at ROM load, never changes during a session.</summary>
	private static uint _cachedRomCrc32;

	/// <summary>
	/// Called when a ROM is loaded. Starts background CDL recording if enabled.
	/// </summary>
	public static void OnRomLoaded(RomInfo romInfo) {
		Log.Info($"[BackgroundPansy] OnRomLoaded: {romInfo.GetRomName()}");
		_currentRomInfo = romInfo;

		// Cache ROM CRC32 once at load time — avoids re-hashing the entire ROM on every auto-save
		_cachedRomCrc32 = PansyExporter.CalculateRomCrc32(romInfo);

		if (!ConfigManager.Config.Debug.Integration.BackgroundCdlRecording) {
			Log.Info("[BackgroundPansy] Background recording disabled in config");
			return;
		}

		StartCdlRecording();
		StartAutoSaveTimer();
	}

	/// <summary>
	/// Called when a ROM is unloaded. Saves final Pansy file and stops recording.
	/// </summary>
	public static void OnRomUnloaded() {
		Log.Info("[BackgroundPansy] OnRomUnloaded");
		StopAutoSaveTimer();

		if (_currentRomInfo is not null && ConfigManager.Config.Debug.Integration.SavePansyOnRomUnload) {
			ExportPansy();
		}

		StopCdlRecording();
		_currentRomInfo = null;
		_cachedRomCrc32 = 0;
	}

	/// <summary>
	/// Start lightweight CDL recording without the full debugger.
	/// Uses LightweightCdlRecorder (~15ns per instruction) instead of
	/// InitializeDebugger() (~200-700ns per instruction with 30-50% overhead).
	/// </summary>
	private static void StartCdlRecording() {
		if (_currentRomInfo is null) return;

		try {
			Log.Info("[BackgroundPansy] Starting lightweight CDL recording");
			DebugApi.StartLightweightCdl();
			_cdlEnabled = true;
		} catch (Exception ex) {
			Log.Error(ex, "[BackgroundPansy] Failed to start lightweight CDL recording");
		}
	}

	/// <summary>
	/// Stop lightweight CDL recording.
	/// </summary>
	private static void StopCdlRecording() {
		if (!_cdlEnabled) return;

		try {
			Log.Info("[BackgroundPansy] Stopping lightweight CDL recording");
			DebugApi.StopLightweightCdl();
			_cdlEnabled = false;
		} catch (Exception ex) {
			Log.Error(ex, "[BackgroundPansy] Failed to stop lightweight CDL recording");
		}
	}

	/// <summary>
	/// Start the periodic auto-save timer.
	/// </summary>
	private static void StartAutoSaveTimer() {
		int intervalMinutes = ConfigManager.Config.Debug.Integration.AutoSaveIntervalMinutes;
		if (intervalMinutes <= 0) {
			Log.Info("[BackgroundPansy] Auto-save disabled (interval = 0)");
			return;
		}

		int intervalMs = intervalMinutes * 60 * 1000;
		Log.Info($"[BackgroundPansy] Starting auto-save timer: {intervalMinutes} minutes");

		_autoSaveTimer?.Dispose();
		_autoSaveTimer = new Timer(OnAutoSaveTick, null, intervalMs, intervalMs);
	}

	/// <summary>
	/// Stop the periodic auto-save timer.
	/// </summary>
	private static void StopAutoSaveTimer() {
		if (_autoSaveTimer is not null) {
			Log.Info("[BackgroundPansy] Stopping auto-save timer");
			_autoSaveTimer.Dispose();
			_autoSaveTimer = null;
		}
	}

	/// <summary>
	/// Timer callback - runs Pansy export on threadpool thread (not UI thread).
	/// File I/O should never block the UI thread.
	/// </summary>
	private static void OnAutoSaveTick(object? state) {
		Log.Info("[BackgroundPansy] Auto-save tick");
		Task.Run(() => ExportPansy());
	}

	/// <summary>
	/// Export the current Pansy file.
	/// </summary>
	private static void ExportPansy() {
		if (_currentRomInfo is null || string.IsNullOrEmpty(_currentRomInfo.RomPath)) {
			Log.Info("[BackgroundPansy] Cannot export - no ROM loaded");
			return;
		}

		try {
			var memoryType = _currentRomInfo.ConsoleType.GetMainCpuType().GetPrgRomMemoryType();
			Log.Info($"[BackgroundPansy] Exporting Pansy for memory type: {memoryType}");
			PansyExporter.AutoExport(_currentRomInfo, memoryType, _cachedRomCrc32);
			Log.Info("[BackgroundPansy] Export complete");
		} catch (Exception ex) {
			Log.Error(ex, "[BackgroundPansy] Export failed");
		}
	}

	/// <summary>
	/// Force an immediate Pansy export (for manual trigger).
	/// </summary>
	public static void ForceExport() {
		ExportPansy();
	}
}
