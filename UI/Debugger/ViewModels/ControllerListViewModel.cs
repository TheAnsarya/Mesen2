using System;
using System.Collections.Generic;
using Avalonia.Controls;
using Nexen.Interop;
using Nexen.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Debugger.ViewModels {
	public class ControllerListViewModel : ViewModelBase {
		[Reactive] public List<ControllerInputViewModel> Controllers { get; set; } = new();

		[Obsolete("For designer only")]
		public ControllerListViewModel() : this(ConsoleType.Snes) { }

		public ControllerListViewModel(ConsoleType consoleType) {
			if (Design.IsDesignMode) {
				return;
			}

			foreach (int index in DebugApi.GetAvailableInputOverrides()) {
				Controllers.Add(new ControllerInputViewModel(consoleType, index));
			}
		}

		public void SetInputOverrides() {
			foreach (ControllerInputViewModel controller in Controllers) {
				controller.SetInputOverrides();
			}
		}
	}
}
