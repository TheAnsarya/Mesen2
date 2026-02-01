using Avalonia.Controls;
using Nexen.Config;
using Nexen.Utilities;
using ReactiveUI.Fody.Helpers;

namespace Nexen.ViewModels {
	public class EmulationConfigViewModel : DisposableViewModel {
		[Reactive] public EmulationConfig Config { get; set; }
		[Reactive] public EmulationConfig OriginalConfig { get; set; }

		public EmulationConfigViewModel() {
			Config = ConfigManager.Config.Emulation;
			OriginalConfig = Config.Clone();

			if (Design.IsDesignMode) {
				return;
			}

			AddDisposable(ReactiveHelper.RegisterRecursiveObserver(Config, (s, e) => Config.ApplyConfig()));
		}
	}
}
