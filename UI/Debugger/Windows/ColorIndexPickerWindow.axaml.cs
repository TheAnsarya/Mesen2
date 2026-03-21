using System;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using Nexen.Config;
using Nexen.Debugger.Controls;
using Nexen.Interop;
using Nexen.Utilities;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Debugger.Windows; 
public class ColorIndexPickerWindow : NexenWindow {
	public UInt32[] Palette { get; set; } = [];
	public int SelectedPalette { get; set; }
	public int BlockSize { get; set; } = 24;
	public int ColumnCount { get; set; } = 16;

	[Obsolete("For designer only")]
	public ColorIndexPickerWindow() : this(CpuType.Nes, 0) { }

	public ColorIndexPickerWindow(CpuType cpuType, int selectedPalette) {
		SelectedPalette = selectedPalette;
		switch (cpuType) {
			case CpuType.Nes:
				Palette = ConfigManager.Config.Nes.UserPalette;
				break;

			case CpuType.Pce:
				Palette = ConfigManager.Config.PcEngine.Palette;
				break;

			case CpuType.Gameboy:
				Palette = selectedPalette < 4
					? ConfigManager.Config.Gameboy.BgColors
					: selectedPalette < 8 ? ConfigManager.Config.Gameboy.Obj0Colors : ConfigManager.Config.Gameboy.Obj1Colors;

				ColumnCount = 4;
				SelectedPalette = selectedPalette % 4;
				break;

			case CpuType.Sms:
				Palette = GenerateSmsPalette();
				ColumnCount = 8;
				break;

			case CpuType.Ws:
				Palette = GenerateWsPalette();
				ColumnCount = 8;
				break;

			case CpuType.Lynx:
				Palette = GenerateLynxPalette();
				ColumnCount = 16;
				break;

			case CpuType.Atari2600:
				Palette = GenerateAtari2600Palette();
				ColumnCount = 16;
				break;

			default:
				throw new NotImplementedException();
		}

		InitializeComponent();
#if DEBUG
		this.AttachDevTools();
#endif
	}

	private static UInt32[] GenerateWsPalette() {
		UInt32[] pal = new UInt32[8];
		WsState state = DebugApi.GetConsoleState<WsState>(ConsoleType.Ws);
		for (int i = 0; i < 8; i++) {
			int b = state.Ppu.BwShades[i] ^ 0x0F;
			pal[i] = 0xFF000000 | (UInt32)(b | (b << 4) | (b << 8) | (b << 12) | (b << 16) | (b << 20) | (b << 24));
		}

		return pal;
	}

	private static UInt32[] GenerateSmsPalette() {
		UInt32[] pal = new UInt32[0x40];
		for (int i = 0; i < 0x40; i++) {
			pal[i] = Rgb222ToArgb((byte)i);
		}

		return pal;
	}

	private static UInt32[] GenerateLynxPalette() {
		// Lynx has 16 palette entries, read from current state
		LynxState state = DebugApi.GetConsoleState<LynxState>(ConsoleType.Lynx);
		UInt32[] pal = new UInt32[16];
		for (int i = 0; i < 16; i++) {
			pal[i] = state.Mikey.Palette[i] | 0xFF000000;
		}

		return pal;
	}

	private static UInt32[] GenerateAtari2600Palette() {
		// Atari 2600 NTSC TIA palette: 128 colors (upper nibble=hue, lower nibble bits 1-3=luminance)
		// Generate a representative palette showing all 128 unique color values
		UInt32[] pal = new UInt32[128];
		for (int i = 0; i < 128; i++) {
			byte color = (byte)(i << 1);
			// Use readout from emulator's palette (the TIA generates colors internally)
			int r = ((color >> 4) & 0x0f) * 17;
			int g = ((color >> 2) & 0x03) * 85;
			int b = (color & 0x0f) * 17;
			pal[i] = 0xff000000 | (uint)(r << 16) | (uint)(g << 8) | (uint)b;
		}

		return pal;
	}

	private static byte Rgb222To8Bit(byte color) {
		return (byte)((byte)(color << 6) | (byte)(color << 4) | (byte)(color << 2) | color);
	}

	private static UInt32 Rgb222ToArgb(byte rgb222) {
		byte b = Rgb222To8Bit((byte)(rgb222 >> 4));
		byte g = Rgb222To8Bit((byte)((rgb222 >> 2) & 0x3));
		byte r = Rgb222To8Bit((byte)(rgb222 & 0x3));

		return 0xFF000000 | (UInt32)(r << 16) | (UInt32)(g << 8) | b;
	}

	private void InitializeComponent() {
		AvaloniaXamlLoader.Load(this);
	}

	protected override void OnOpened(EventArgs e) {
		base.OnOpened(e);
		if (Owner != null) {
			WindowExtensions.CenterWindow(this, Owner);
		}
	}

	private void Cancel_OnClick(object sender, RoutedEventArgs e) {
		Close(null!);
	}

	private void PaletteColor_OnClick(object sender, PaletteSelector.ColorClickEventArgs e) {
		Close(e.ColorIndex);
	}
}
