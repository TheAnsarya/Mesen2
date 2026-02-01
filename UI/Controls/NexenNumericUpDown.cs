using System;
using Avalonia.Controls;

namespace Nexen.Controls;

public class NexenNumericUpDown : NumericUpDown {
	protected override Type StyleKeyOverride => typeof(NumericUpDown);

	protected override void OnTextChanged(string? oldValue, string? newValue) {
		if (newValue is null or "") {
			//Prevent displaying invalid cast error/breaking the layout when user clears the content of the control
			Text = "0";
		}

		base.OnTextChanged(oldValue, newValue);
	}
}
