using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views {
	public class PceControllerView : UserControl {
		public PceControllerView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
