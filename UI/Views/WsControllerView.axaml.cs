using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views;

public class WsControllerView : UserControl {
	public WsControllerView() {
		InitializeComponent();
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
	}
}
