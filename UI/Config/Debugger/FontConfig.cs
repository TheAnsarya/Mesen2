using System;
using Avalonia.Media;
using Nexen.ViewModels;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config; 
public sealed class FontConfig : BaseConfig<FontConfig> {
	[Reactive] public string FontFamily { get; set; } = "";
	[Reactive] public double FontSize { get; set; } = 12;
}
