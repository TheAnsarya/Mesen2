using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views; 
public class PceInputConfigView : UserControl {
	public PceInputConfigView() {
		InitializeComponent();
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
	}
}
