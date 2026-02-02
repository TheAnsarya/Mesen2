using System.Collections.Generic;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class RegisterViewerConfig : BaseWindowConfig<RegisterViewerConfig> {
	[Reactive] public RefreshTimingConfig RefreshTiming { get; set; } = new();
	[Reactive] public List<int> ColumnWidths { get; set; } = new();
}
