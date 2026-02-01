using System.Reactive;
using System.Reactive.Linq;
using Avalonia;
using Avalonia.Media;
using Nexen.Debugger;
using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config {
	public class SmsDebuggerConfig : ViewModelBase {
		[Reactive] public bool BreakOnNopLoad { get; set; } = false;
	}
}
