using System.Reactive;
using System.Reactive.Linq;
using Avalonia;
using Avalonia.Media;
using Nexen.Debugger;
using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public sealed class WsDebuggerConfig : ViewModelBase {
	[Reactive] public bool BreakOnUndefinedOpCode { get; set; } = false;
}
