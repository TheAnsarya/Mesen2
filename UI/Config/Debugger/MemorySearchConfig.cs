using System.Collections.Generic;
using Nexen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public sealed class MemorySearchConfig : BaseWindowConfig<MemorySearchConfig> {
	[Reactive] public List<int> ColumnWidths { get; set; } = new();
}
