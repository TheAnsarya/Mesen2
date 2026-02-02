using System;
using System.Reactive.Linq;
using Avalonia.Threading;
using Nexen.Config;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Nexen.ViewModels; 
public class CvInputConfigViewModel : DisposableViewModel {
	[Reactive] public CvConfig Config { get; set; }

	public Enum[] AvailableControllerTypesP12 => new Enum[] {
		ControllerType.None,
		ControllerType.ColecoVisionController,
	};

	[Obsolete("For designer only")]
	public CvInputConfigViewModel() : this(new CvConfig()) { }

	public CvInputConfigViewModel(CvConfig config) {
		Config = config;
	}
}
