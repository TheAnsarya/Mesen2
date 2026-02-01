using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views {
	public class SmsInputConfigView : UserControl {
		public SmsInputConfigView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
