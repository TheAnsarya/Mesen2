using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.VisualTree;

namespace Mesen.Utilities {
	static class ControlExtensions {
		public static bool IsParentWindowFocused(this Control ctrl) {
			return ctrl.GetVisualRoot() is WindowBase { IsKeyboardFocusWithin: true };
		}
	}
}
