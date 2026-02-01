using System;
using System.Reactive.Linq;
using Avalonia.Threading;
using Nexen.Config;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Nexen.ViewModels {
	public class SmsInputConfigViewModel : DisposableViewModel {
		[Reactive] public SmsConfig Config { get; set; }

		public Enum[] AvailableControllerTypesP12 => new Enum[] {
			ControllerType.None,
			ControllerType.SmsController,
			ControllerType.SmsLightPhaser,
		};

		[Obsolete("For designer only")]
		public SmsInputConfigViewModel() : this(new SmsConfig()) { }

		public SmsInputConfigViewModel(SmsConfig config) {
			Config = config;
		}
	}
}
