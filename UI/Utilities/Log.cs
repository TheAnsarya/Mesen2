using System;
using System.IO;
using Nexen.Config;
using Serilog;
using Serilog.Events;

namespace Nexen.Utilities;

/// <summary>
/// Simple logging wrapper using Serilog for file-based logging.
/// Logs are written to the Nexen home folder as nexen-log.txt.
/// </summary>
public static class Log {
	private static bool _initialized = false;
	private static readonly object _lock = new();

	/// <summary>
	/// Gets the path to the log file.
	/// </summary>
	public static string LogFilePath => Path.Combine(
		ConfigManager.HomeFolder ?? Path.GetTempPath(),
		"nexen-log.txt"
	);

	/// <summary>
	/// Initializes the logging system. Safe to call multiple times.
	/// </summary>
	public static void Initialize() {
		if (_initialized) return;

		lock (_lock) {
			if (_initialized) return;

			try {
				string logPath = LogFilePath;

				// Ensure directory exists
				string? dir = Path.GetDirectoryName(logPath);
				if (dir != null) {
					Directory.CreateDirectory(dir);
				}

				Serilog.Log.Logger = new LoggerConfiguration()
					.MinimumLevel.Debug()
					.WriteTo.File(
						logPath,
						rollingInterval: RollingInterval.Day,
						retainedFileCountLimit: 7,
						outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff} [{Level:u3}] {Message:lj}{NewLine}{Exception}",
						flushToDiskInterval: TimeSpan.FromSeconds(1)
					)
					.WriteTo.Console(
						outputTemplate: "[{Timestamp:HH:mm:ss} {Level:u3}] {Message:lj}{NewLine}{Exception}"
					)
					.CreateLogger();

				_initialized = true;
				Info("=== Nexen logging initialized ===");
				Info($"Log file: {logPath}");
			} catch (Exception ex) {
				// If logging fails to initialize, write to debug output
				System.Diagnostics.Debug.WriteLine($"Failed to initialize logging: {ex}");
			}
		}
	}

	/// <summary>
	/// Ensures logging is initialized before writing.
	/// </summary>
	private static void EnsureInitialized() {
		if (!_initialized) {
			Initialize();
		}
	}

	/// <summary>
	/// Logs a debug message.
	/// </summary>
	public static void Debug(string message) {
		EnsureInitialized();
		Serilog.Log.Debug(message);
	}

	/// <summary>
	/// Logs an informational message.
	/// </summary>
	public static void Info(string message) {
		EnsureInitialized();
		Serilog.Log.Information(message);
	}

	/// <summary>
	/// Logs a warning message.
	/// </summary>
	public static void Warn(string message) {
		EnsureInitialized();
		Serilog.Log.Warning(message);
	}

	/// <summary>
	/// Logs an error message.
	/// </summary>
	public static void Error(string message) {
		EnsureInitialized();
		Serilog.Log.Error(message);
	}

	/// <summary>
	/// Logs an error with exception details.
	/// </summary>
	public static void Error(Exception ex, string message) {
		EnsureInitialized();
		Serilog.Log.Error(ex, message);
	}

	/// <summary>
	/// Logs a fatal error that will likely crash the application.
	/// </summary>
	public static void Fatal(string message) {
		EnsureInitialized();
		Serilog.Log.Fatal(message);
	}

	/// <summary>
	/// Logs a fatal error with exception details.
	/// </summary>
	public static void Fatal(Exception ex, string message) {
		EnsureInitialized();
		Serilog.Log.Fatal(ex, message);
	}

	/// <summary>
	/// Flushes any buffered log entries and closes the log.
	/// Call this before application exit.
	/// </summary>
	public static void CloseAndFlush() {
		Serilog.Log.CloseAndFlush();
	}
}
