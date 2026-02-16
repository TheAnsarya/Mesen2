using System;
using System.Reactive;
using Avalonia;
using Avalonia.Controls;
using Avalonia.VisualTree;
using Nexen.Config;
using Nexen.Utilities;
using Nexen.Windows;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Nexen.ViewModels;
public sealed class LynxConfigViewModel : DisposableViewModel {
	[Reactive] public LynxConfig Config { get; set; }
	[Reactive] public LynxConfig OriginalConfig { get; set; }
	[Reactive] public LynxConfigTab SelectedTab { get; set; } = 0;

	public ReactiveCommand<Button, Unit> SetupPlayer { get; }

	public LynxConfigViewModel() {
		Config = ConfigManager.Config.Lynx;
		OriginalConfig = Config.Clone();

		IObservable<bool> buttonEnabled = this.WhenAnyValue(x => x.Config.Controller.Type, x => x.CanConfigure());
		SetupPlayer = ReactiveCommand.Create<Button>(btn => this.OpenSetup(btn, 0), buttonEnabled);

		if (Design.IsDesignMode) {
			return;
		}

		AddDisposable(ReactiveHelper.RegisterRecursiveObserver(Config, (s, e) => Config.ApplyConfig()));
	}

	private async void OpenSetup(Button btn, int port) {
		PixelPoint startPosition = btn.PointToScreen(new Point(-7, btn.Bounds.Height));
		ControllerConfigWindow wnd = new ControllerConfigWindow();
		ControllerConfig orgCfg = Config.Controller;
		ControllerConfig cfg = Config.Controller.Clone();
		wnd.DataContext = new ControllerConfigViewModel(ControllerType.LynxController, cfg, orgCfg, port);

		if (await wnd.ShowDialogAtPosition<bool>(btn.GetVisualRoot() as Visual, startPosition)) {
			Config.Controller = cfg;
		}
	}
}

public enum LynxConfigTab {
	General,
	Audio,
	Emulation,
	Input,
	Video
}
