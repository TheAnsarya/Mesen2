using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace Nexen.Views; 
public class SnesControllerView : UserControl {
	public SnesControllerView() {
		InitializeComponent();
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
	}
}
