using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Nexen.Interop;

namespace Nexen.Views {
	public class AudioConfigView : UserControl {
		public AudioConfigView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
