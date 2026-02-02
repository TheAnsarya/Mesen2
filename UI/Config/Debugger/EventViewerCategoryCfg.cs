using System;
using Avalonia.Media;
using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class EventViewerCategoryCfg : ViewModelBase {
	[Reactive] public bool Visible { get; set; } = true;
	[Reactive] public UInt32 Color { get; set; }

	public EventViewerCategoryCfg() { }

	public EventViewerCategoryCfg(Color color) {
		Color = color.ToUInt32();
	}

	public static implicit operator InteropEventViewerCategoryCfg(EventViewerCategoryCfg cfg) {
		return new InteropEventViewerCategoryCfg() {
			Visible = cfg.Visible,
			Color = cfg.Color
		};
	}
}
