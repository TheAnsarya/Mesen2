using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views {
	public class NesInputConfigView : UserControl {
		public NesInputConfigView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
