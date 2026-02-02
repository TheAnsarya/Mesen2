using System;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class HistoryViewerConfig : BaseWindowConfig<HistoryViewerConfig> {
	[Reactive] public int Volume { get; set; } = 100;
}
