using System;
using System.IO;
using System.Reflection;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Controls.Platform;
using Avalonia.Markup.Xaml;
using Avalonia.Styling;
using Avalonia.Threading;
using Nexen.Config;
using Nexen.Interop;
using Nexen.Localization;
using Nexen.Utilities;
using Nexen.ViewModels;
using Nexen.Windows;

namespace Nexen {
	public class App : Application {
		public static bool ShowConfigWindow { get; set; }

		public override void Initialize() {
			RequestedThemeVariant = Design.IsDesignMode || ShowConfigWindow
				? ThemeVariant.Light
				: ConfigManager.Config.Preferences.Theme == NexenTheme.Dark ? ThemeVariant.Dark : ThemeVariant.Light;

			Dispatcher.UIThread.UnhandledException += (s, e) => {
				NexenMsgBox.ShowException(e.Exception);
				e.Handled = true;
			};

			AvaloniaXamlLoader.Load(this);
			ResourceHelper.LoadResources();
		}

		public override void OnFrameworkInitializationCompleted() {
			if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop) {
				if (ShowConfigWindow) {
					new PreferencesConfig().InitializeFontDefaults();
					desktop.MainWindow = new SetupWizardWindow();
				} else {
					//Test if the core can be loaded, and display an error message popup if not
					try {
						EmuApi.TestDll();
					} catch (Exception ex) {
						bool sdlMissing = ex.Message.Contains("SDL2", StringComparison.InvariantCultureIgnoreCase);

						string errorMessage = sdlMissing
							? ResourceHelper.GetMessage("UnableToStartMissingSdl", ex.Message)
							: ResourceHelper.GetMessage("UnableToStartMissingDependencies", ex.Message + Environment.NewLine + ex.StackTrace);
						MessageBox.Show(null, errorMessage, "Mesen", MessageBoxButtons.OK, MessageBoxIcon.Error, out MessageBox msgbox);
						desktop.MainWindow = msgbox;
						base.OnFrameworkInitializationCompleted();
						return;
					}

					try {
						desktop.MainWindow = new MainWindow();
					} catch {
						//Something broke when trying to load the main window, the settings file might be invalid/broken, try to reset them
						Configuration.BackupSettings(ConfigManager.ConfigFile);
						ConfigManager.ResetSettings(false);
						desktop.MainWindow = new MainWindow();
					}
				}
			}

			base.OnFrameworkInitializationCompleted();
		}
	}
}
