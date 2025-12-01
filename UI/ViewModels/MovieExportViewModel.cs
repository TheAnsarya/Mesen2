using Avalonia.Media;
using Mesen.Config;
using Mesen.Interop;
using Mesen.Localization;
using Mesen.Utilities;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reactive.Linq;
using System.Threading.Tasks;

namespace Mesen.ViewModels
{
	public class MovieExportViewModel : ViewModelBase
	{
		[Reactive] public string SourcePath { get; set; } = "";
		[Reactive] public string ExportPath { get; set; } = "";
		[Reactive] public string SelectedFormat { get; set; } = "";
		[Reactive] public string StatusMessage { get; set; } = "";
		[Reactive] public IBrush StatusColor { get; set; } = Brushes.Gray;
		[Reactive] public bool CanExport { get; set; } = false;

		public List<string> AvailableFormats { get; } = new List<string>
		{
			"SNES9x (.smv)",
			"LSNES (.lsmv)",
			"FCEUX (.fm2)",
			"BizHawk (.bk2)"
		};

		public MovieExportViewModel()
		{
			SelectedFormat = AvailableFormats[0];
			
			// Update CanExport when paths change
			this.WhenAnyValue(x => x.SourcePath, x => x.ExportPath)
				.Subscribe(_ => UpdateCanExport());
		}

		public MovieExportViewModel(string sourcePath) : this()
		{
			SourcePath = sourcePath;
			
			// Set default export path based on source
			if(!string.IsNullOrEmpty(sourcePath))
			{
				string baseName = Path.GetFileNameWithoutExtension(sourcePath);
				ExportPath = Path.Combine(ConfigManager.MovieFolder, baseName + GetSelectedExtension());
			}
		}

		private void UpdateCanExport()
		{
			CanExport = !string.IsNullOrEmpty(SourcePath) && 
			            !string.IsNullOrEmpty(ExportPath) &&
			            File.Exists(SourcePath);
			
			if(!string.IsNullOrEmpty(SourcePath) && !File.Exists(SourcePath))
			{
				StatusMessage = ResourceHelper.GetMessage("MovieSourceNotFound");
				StatusColor = Brushes.Red;
			}
			else
			{
				StatusMessage = "";
			}
		}

		public string GetSelectedExtension()
		{
			return SelectedFormat switch
			{
				"SNES9x (.smv)" => ".smv",
				"LSNES (.lsmv)" => ".lsmv",
				"FCEUX (.fm2)" => ".fm2",
				"BizHawk (.bk2)" => ".bk2",
				_ => ".smv"
			};
		}

		public async Task<bool> Export()
		{
			if(!CanExport)
			{
				return false;
			}

			StatusMessage = ResourceHelper.GetMessage("MovieExporting");
			StatusColor = Brushes.Blue;

			try
			{
				// Get the path to the MovieConverter executable
				string converterPath = GetConverterPath();
				
				if(!File.Exists(converterPath))
				{
					StatusMessage = ResourceHelper.GetMessage("MovieConverterNotFound");
					StatusColor = Brushes.Red;
					return false;
				}

				// Run the MovieConverter CLI
				var startInfo = new ProcessStartInfo
				{
					FileName = converterPath,
					Arguments = $"\"{SourcePath}\" \"{ExportPath}\"",
					UseShellExecute = false,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					CreateNoWindow = true
				};

				using var process = new Process { StartInfo = startInfo };
				process.Start();
				
				string output = await process.StandardOutput.ReadToEndAsync();
				string error = await process.StandardError.ReadToEndAsync();
				
				await process.WaitForExitAsync();

				if(process.ExitCode == 0)
				{
					StatusMessage = ResourceHelper.GetMessage("MovieExportSuccess");
					StatusColor = Brushes.Green;
					return true;
				}
				else
				{
					StatusMessage = string.IsNullOrEmpty(error) ? output : error;
					StatusColor = Brushes.Red;
					return false;
				}
			}
			catch(Exception ex)
			{
				StatusMessage = ex.Message;
				StatusColor = Brushes.Red;
				return false;
			}
		}

		private string GetConverterPath()
		{
			// Get the directory where the main executable is located
			string exeDir = AppContext.BaseDirectory;
			
			// Try multiple possible paths for the converter
			string[] possiblePaths = new[]
			{
				Path.Combine(exeDir, "MesenMovieConverter.exe"),
				Path.Combine(exeDir, "MesenMovieConverter"),
				Path.Combine(exeDir, "..", "MovieConverter", "MesenMovieConverter.exe"),
				Path.Combine(exeDir, "..", "MovieConverter", "MesenMovieConverter"),
			};

			foreach(string path in possiblePaths)
			{
				string fullPath = Path.GetFullPath(path);
				if(File.Exists(fullPath))
				{
					return fullPath;
				}
			}

			// Default to same directory as main app
			return Path.Combine(exeDir, "MesenMovieConverter.exe");
		}
	}
}
