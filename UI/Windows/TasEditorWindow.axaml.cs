using System;
using System.Collections.Generic;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using Nexen.Controls;
using Nexen.ViewModels;

namespace Nexen.Windows;

/// <summary>
/// TAS Editor window for frame-by-frame movie editing.
/// </summary>
public class TasEditorWindow : NexenWindow {
	private ListBox _frameList = null!;
	private PianoRollControl _pianoRoll = null!;

	public TasEditorWindow() {
		InitializeComponent();
#if DEBUG
		this.AttachDevTools();
#endif
		DataContext = new TasEditorViewModel();

		if (DataContext is TasEditorViewModel vm) {
			vm.InitActions(this);
		}

		SetupPianoRollEvents();
	}

	public TasEditorWindow(TasEditorViewModel viewModel) {
		InitializeComponent();
#if DEBUG
		this.AttachDevTools();
#endif
		DataContext = viewModel;
		viewModel.InitActions(this);

		SetupPianoRollEvents();
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
		_frameList = this.FindControl<ListBox>("FrameList")!;
		_pianoRoll = this.FindControl<PianoRollControl>("PianoRoll")!;
	}

	/// <summary>
	/// Sets up event handlers for the piano roll control.
	/// </summary>
	private void SetupPianoRollEvents() {
		_pianoRoll.CellClicked += OnPianoRollCellClicked;
		_pianoRoll.CellsPainted += OnPianoRollCellsPainted;
		_pianoRoll.SelectionChanged += OnPianoRollSelectionChanged;
	}

	/// <summary>
	/// Gets the view model for this window.
	/// </summary>
	public TasEditorViewModel? ViewModel => DataContext as TasEditorViewModel;

	/// <summary>
	/// Handles button clicks for controller input.
	/// Toggles the button state for the currently selected frame.
	/// </summary>
	private void OnButtonClick(object? sender, RoutedEventArgs e) {
		if (sender is Button button && button.Tag is string buttonName && ViewModel != null) {
			ViewModel.ToggleButton(0, buttonName);
		}
	}

	/// <summary>
	/// Called when the window is initialized.
	/// </summary>
	protected override void OnInitialized() {
		base.OnInitialized();

		// Set up keyboard shortcuts
		KeyDown += (s, e) => {
			if (ViewModel == null) {
				return;
			}

			// Ctrl+key shortcuts
			if (e.KeyModifiers.HasFlag(Avalonia.Input.KeyModifiers.Control)) {
				switch (e.Key) {
					case Avalonia.Input.Key.Z:
						ViewModel.Undo();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.Y:
						ViewModel.Redo();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.S:
						_ = ViewModel.SaveFileAsync();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.O:
						_ = ViewModel.OpenFileAsync();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.C:
						ViewModel.Copy();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.V:
						ViewModel.Paste();
						e.Handled = true;
						return;
					case Avalonia.Input.Key.X:
						ViewModel.Cut();
						e.Handled = true;
						return;
				}
			}

			// Frame navigation with arrow keys when ListBox doesn't have focus
			if (!_frameList.IsFocused) {
				switch (e.Key) {
					case Avalonia.Input.Key.PageUp:
						ViewModel.SelectedFrameIndex = Math.Max(0, ViewModel.SelectedFrameIndex - 10);
						e.Handled = true;
						break;
					case Avalonia.Input.Key.PageDown:
						ViewModel.SelectedFrameIndex = Math.Min(ViewModel.Frames.Count - 1, ViewModel.SelectedFrameIndex + 10);
						e.Handled = true;
						break;
					case Avalonia.Input.Key.Home:
						ViewModel.SelectedFrameIndex = 0;
						e.Handled = true;
						break;
					case Avalonia.Input.Key.End:
						ViewModel.SelectedFrameIndex = ViewModel.Frames.Count - 1;
						e.Handled = true;
						break;
					case Avalonia.Input.Key.Space:
						ViewModel.TogglePlayback();
						e.Handled = true;
						break;
					case Avalonia.Input.Key.F:
						ViewModel.FrameAdvance();
						e.Handled = true;
						break;
					case Avalonia.Input.Key.R:
						ViewModel.FrameRewind();
						e.Handled = true;
						break;
					case Avalonia.Input.Key.Insert:
						ViewModel.InsertFrames();
						e.Handled = true;
						break;
					case Avalonia.Input.Key.Delete:
						ViewModel.DeleteFrames();
						e.Handled = true;
						break;
				}
			}
		};
	}

	/// <summary>
	/// Called when the window is closing. Prompts to save unsaved changes.
	/// </summary>
	protected override void OnClosing(WindowClosingEventArgs e) {
		if (ViewModel?.HasUnsavedChanges == true) {
			// TODO: Show confirmation dialog
			// For now, allow close without prompting
		}
		base.OnClosing(e);
	}

	#region Toolbar Click Handlers

	private void OnOpenClick(object? sender, RoutedEventArgs e) {
		_ = ViewModel?.OpenFileAsync();
	}

	private void OnSaveClick(object? sender, RoutedEventArgs e) {
		_ = ViewModel?.SaveFileAsync();
	}

	private void OnUndoClick(object? sender, RoutedEventArgs e) {
		ViewModel?.Undo();
	}

	private void OnRedoClick(object? sender, RoutedEventArgs e) {
		ViewModel?.Redo();
	}

	private void OnInsertFrameClick(object? sender, RoutedEventArgs e) {
		ViewModel?.InsertFrames();
	}

	private void OnDeleteFrameClick(object? sender, RoutedEventArgs e) {
		ViewModel?.DeleteFrames();
	}

	private void OnFrameRewindClick(object? sender, RoutedEventArgs e) {
		ViewModel?.FrameRewind();
	}

	private void OnPlayPauseClick(object? sender, RoutedEventArgs e) {
		ViewModel?.TogglePlayback();
	}

	private void OnFrameAdvanceClick(object? sender, RoutedEventArgs e) {
		ViewModel?.FrameAdvance();
	}

	#endregion

	#region Piano Roll Event Handlers

	private void OnPianoRollCellClicked(object? sender, PianoRollCellEventArgs e) {
		if (ViewModel == null) {
			return;
		}

		// Map button index to button name and toggle at the specific frame
		var buttonLabels = _pianoRoll.ButtonLabels ?? GetDefaultButtonLabels();
		if (e.ButtonIndex >= 0 && e.ButtonIndex < buttonLabels.Count) {
			string buttonName = MapButtonLabelToName(buttonLabels[e.ButtonIndex]);
			ViewModel.ToggleButtonAtFrame(e.Frame, 0, buttonName, e.NewState);
		}
	}

	private void OnPianoRollCellsPainted(object? sender, PianoRollPaintEventArgs e) {
		if (ViewModel == null) {
			return;
		}

		var buttonLabels = _pianoRoll.ButtonLabels ?? GetDefaultButtonLabels();
		if (e.ButtonIndex >= 0 && e.ButtonIndex < buttonLabels.Count) {
			string buttonName = MapButtonLabelToName(buttonLabels[e.ButtonIndex]);
			foreach (int frame in e.Frames) {
				ViewModel.SetButtonAtFrame(frame, 0, buttonName, e.PaintValue);
			}
			ViewModel.RefreshFrames();
		}
	}

	private void OnPianoRollSelectionChanged(object? sender, PianoRollSelectionEventArgs e) {
		if (ViewModel != null) {
			ViewModel.SelectedFrameIndex = e.SelectionStart;
		}
	}

	private static IReadOnlyList<string> GetDefaultButtonLabels() =>
		new[] { "A", "B", "X", "Y", "L", "R", "↑", "↓", "←", "→", "ST", "SE" };

	private static string MapButtonLabelToName(string label) {
		return label.ToUpperInvariant() switch {
			"↑" => "UP",
			"↓" => "DOWN",
			"←" => "LEFT",
			"→" => "RIGHT",
			"ST" => "START",
			"SE" => "SELECT",
			_ => label.ToUpperInvariant()
		};
	}

	#endregion
}
