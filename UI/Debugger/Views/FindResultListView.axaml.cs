using System;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using DataBoxControl;
using Nexen.Config;
using Nexen.Debugger;
using Nexen.Debugger.Utilities;
using Nexen.Debugger.ViewModels;
using Nexen.Debugger.Windows;
using Nexen.Utilities;
using Nexen.ViewModels;

namespace Nexen.Debugger.Views {
	public class FindResultListView : UserControl {
		public FindResultListView() {
			InitializeComponent();
		}

		private void InitializeComponent() {
			AvaloniaXamlLoader.Load(this);
		}

		protected override void OnDataContextChanged(EventArgs e) {
			if (DataContext is FindResultListViewModel vm) {
				vm.InitContextMenu(this);
			}

			base.OnDataContextChanged(e);
		}

		private void OnCellDoubleClick(DataBoxCell cell) {
			if (DataContext is FindResultListViewModel listModel && cell.DataContext is FindResultViewModel result) {
				listModel.GoToResult(result);
			}
		}
	}
}
