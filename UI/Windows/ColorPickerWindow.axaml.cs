using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;

namespace Nexen.Windows; 
public class ColorPickerWindow : NexenWindow {
	public ColorPickerWindow() {
		InitializeComponent();
#if DEBUG
		this.AttachDevTools();
#endif
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
	}

	private void Ok_OnClick(object sender, RoutedEventArgs e) {
		Close(true);
	}

	private void Cancel_OnClick(object sender, RoutedEventArgs e) {
		Close(false);
	}
}
