using System.Collections.Generic;
using Mesen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Mesen.Config {
	public class MemorySearchConfig : BaseWindowConfig<MemorySearchConfig> {
		[Reactive] public List<int> ColumnWidths { get; set; } = new();
	}
}
