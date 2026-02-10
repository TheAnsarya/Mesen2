using System;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public sealed class HistoryViewerConfig : BaseWindowConfig<HistoryViewerConfig> {
	[Reactive] public int Volume { get; set; } = 100;
}
