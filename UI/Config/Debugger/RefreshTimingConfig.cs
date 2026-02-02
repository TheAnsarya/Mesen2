using Nexen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class RefreshTimingConfig : BaseConfig<RefreshTimingConfig> {
	[Reactive] public bool RefreshOnBreakPause { get; set; } = true;
	[Reactive] public bool AutoRefresh { get; set; } = true;

	[Reactive] public int RefreshScanline { get; set; } = 240;
	[Reactive] public int RefreshCycle { get; set; } = 0;

	public RefreshTimingConfig() {
	}
}
