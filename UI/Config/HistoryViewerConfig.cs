using System;
using ReactiveUI.Fody.Helpers;

namespace Mesen.Config {
	public class HistoryViewerConfig : BaseWindowConfig<HistoryViewerConfig> {
		[Reactive] public int Volume { get; set; } = 100;
	}
}
