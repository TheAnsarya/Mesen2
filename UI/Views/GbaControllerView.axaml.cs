using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views {
	public class GbaControllerView : UserControl {
		public GbaControllerView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
