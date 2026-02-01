using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views {
	public class SmsControllerView : UserControl {
		public SmsControllerView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}
	}
}
