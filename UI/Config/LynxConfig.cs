using System;
using System.Runtime.InteropServices;
using Nexen.Interop;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Config;

public sealed class LynxConfig : BaseConfig<LynxConfig> {
	[Reactive] public ConsoleOverrideConfig ConfigOverrides { get; set; } = new();

	[Reactive] public ControllerConfig Controller { get; set; } = new();

	[Reactive] public bool UseBootRom { get; set; } = false;
	[Reactive] public bool AutoRotate { get; set; } = true;
	[Reactive] public bool BlendFrames { get; set; } = false;

	[Reactive] public bool DisableSprites { get; set; } = false;

	[Reactive][MinMax(0, 100)] public UInt32 Channel1Vol { get; set; } = 100;
	[Reactive][MinMax(0, 100)] public UInt32 Channel2Vol { get; set; } = 100;
	[Reactive][MinMax(0, 100)] public UInt32 Channel3Vol { get; set; } = 100;
	[Reactive][MinMax(0, 100)] public UInt32 Channel4Vol { get; set; } = 100;

	public void ApplyConfig() {
		Controller.Type = ControllerType.LynxController;

		ConfigManager.Config.Video.ApplyConfig();

		ConfigApi.SetLynxConfig(new InteropLynxConfig() {
			Controller = Controller.ToInterop(),

			UseBootRom = UseBootRom,
			AutoRotate = AutoRotate,
			BlendFrames = BlendFrames,

			DisableSprites = DisableSprites,

			Channel1Vol = Channel1Vol,
			Channel2Vol = Channel2Vol,
			Channel3Vol = Channel3Vol,
			Channel4Vol = Channel4Vol,
		});
	}

	internal void InitializeDefaults(DefaultKeyMappingType defaultMappings) {
		Controller.InitDefaults(defaultMappings, ControllerType.LynxController);
	}
}

[StructLayout(LayoutKind.Sequential)]
public struct InteropLynxConfig {
	public InteropControllerConfig Controller;

	[MarshalAs(UnmanagedType.I1)] public bool UseBootRom;
	[MarshalAs(UnmanagedType.I1)] public bool AutoRotate;
	[MarshalAs(UnmanagedType.I1)] public bool BlendFrames;

	[MarshalAs(UnmanagedType.I1)] public bool DisableSprites;

	public UInt32 Channel1Vol;
	public UInt32 Channel2Vol;
	public UInt32 Channel3Vol;
	public UInt32 Channel4Vol;
}
