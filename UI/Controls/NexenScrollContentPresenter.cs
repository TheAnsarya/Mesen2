using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Presenters;
using Avalonia.Input;
using Avalonia.LogicalTree;
using Avalonia.Styling;
using DataBoxControl;
using DataBoxControl.Primitives;

namespace Nexen.Controls {
	public class NexenScrollContentPresenter : ScrollContentPresenter {
		protected override Type StyleKeyOverride => typeof(ScrollContentPresenter);

		protected override void OnPointerWheelChanged(PointerWheelEventArgs e) {
			if (!e.KeyModifiers.HasFlag(KeyModifiers.Control)) {
				//Skip event if control is pressed, because this is used to zoom in/out
				base.OnPointerWheelChanged(e);
			}
		}
	}
}
