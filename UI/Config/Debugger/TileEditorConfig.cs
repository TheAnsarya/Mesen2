using Nexen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config {
	public class TileEditorConfig : BaseWindowConfig<TileEditorConfig> {
		[Reactive] public double ImageScale { get; set; } = 8;
		[Reactive] public bool ShowGrid { get; set; } = false;
		[Reactive] public TileBackground Background { get; set; } = TileBackground.Transparent;
	}
}
