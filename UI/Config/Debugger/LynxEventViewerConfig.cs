using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config;

public sealed class LynxEventViewerConfig : ViewModelBase {
	[Reactive] public EventViewerCategoryCfg Irq { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[0]);
	[Reactive] public EventViewerCategoryCfg MarkedBreakpoints { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[1]);

	[Reactive] public EventViewerCategoryCfg MikeyRegisterWrite { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[2]);
	[Reactive] public EventViewerCategoryCfg MikeyRegisterRead { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[3]);
	[Reactive] public EventViewerCategoryCfg SuzyRegisterWrite { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[4]);
	[Reactive] public EventViewerCategoryCfg SuzyRegisterRead { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[5]);
	[Reactive] public EventViewerCategoryCfg AudioRegisterWrite { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[6]);
	[Reactive] public EventViewerCategoryCfg AudioRegisterRead { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[7]);
	[Reactive] public EventViewerCategoryCfg PaletteWrite { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[8]);
	[Reactive] public EventViewerCategoryCfg TimerWrite { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[9]);
	[Reactive] public EventViewerCategoryCfg TimerRead { get; set; } = new EventViewerCategoryCfg(EventViewerColors.Colors[10]);

	[Reactive] public bool ShowPreviousFrameEvents { get; set; } = true;

	public InteropLynxEventViewerConfig ToInterop() {
		return new InteropLynxEventViewerConfig() {
			Irq = this.Irq,
			MarkedBreakpoints = this.MarkedBreakpoints,
			MikeyRegisterWrite = this.MikeyRegisterWrite,
			MikeyRegisterRead = this.MikeyRegisterRead,
			SuzyRegisterWrite = this.SuzyRegisterWrite,
			SuzyRegisterRead = this.SuzyRegisterRead,
			AudioRegisterWrite = this.AudioRegisterWrite,
			AudioRegisterRead = this.AudioRegisterRead,
			PaletteWrite = this.PaletteWrite,
			TimerWrite = this.TimerWrite,
			TimerRead = this.TimerRead,
			ShowPreviousFrameEvents = this.ShowPreviousFrameEvents
		};
	}
}
