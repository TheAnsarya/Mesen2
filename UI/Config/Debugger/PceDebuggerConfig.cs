using System.Reactive;
using System.Reactive.Linq;
using Avalonia;
using Avalonia.Media;
using Nexen.Debugger;
using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public class PceDebuggerConfig : ViewModelBase {
	[Reactive] public bool BreakOnBrk { get; set; } = false;
	[Reactive] public bool BreakOnUnofficialOpCode { get; set; } = false;
	[Reactive] public bool BreakOnInvalidVramAddress { get; set; } = false;
}
