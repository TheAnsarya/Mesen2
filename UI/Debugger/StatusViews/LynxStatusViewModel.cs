using System;
using System.Text;
using Nexen.Interop;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace Nexen.Debugger.StatusViews;

public sealed class LynxStatusViewModel : BaseConsoleStatusViewModel {
	[Reactive] public byte RegA { get; set; }
	[Reactive] public byte RegX { get; set; }
	[Reactive] public byte RegY { get; set; }
	[Reactive] public byte RegSP { get; set; }
	[Reactive] public UInt16 RegPC { get; set; }
	[Reactive] public byte RegPS { get; set; }

	[Reactive] public bool FlagN { get; set; }
	[Reactive] public bool FlagV { get; set; }
	[Reactive] public bool FlagD { get; set; }
	[Reactive] public bool FlagI { get; set; }
	[Reactive] public bool FlagZ { get; set; }
	[Reactive] public bool FlagC { get; set; }

	[Reactive] public UInt16 Cycle { get; private set; }
	[Reactive] public UInt16 Scanline { get; private set; }
	[Reactive] public UInt32 FrameCount { get; private set; }

	[Reactive] public string StackPreview { get; private set; } = "";

	public LynxStatusViewModel() {
		this.WhenAnyValue(x => x.FlagC, x => x.FlagD, x => x.FlagI, x => x.FlagN, x => x.FlagV, x => x.FlagZ).Subscribe(x => RegPS = (byte)(
			(FlagN ? 0x80 : 0) |
			(FlagV ? 0x40 : 0) |
			0x20 | // unused bit always set
			(FlagD ? 0x08 : 0) |
			(FlagI ? 0x04 : 0) |
			(FlagZ ? 0x02 : 0) |
			(FlagC ? 0x01 : 0)
		));

		this.WhenAnyValue(x => x.RegPS).Subscribe(x => {
			using var delayNotifs = DelayChangeNotifications();
			FlagN = (x & 0x80) != 0;
			FlagV = (x & 0x40) != 0;
			FlagD = (x & 0x08) != 0;
			FlagI = (x & 0x04) != 0;
			FlagZ = (x & 0x02) != 0;
			FlagC = (x & 0x01) != 0;
		});
	}

	protected override void InternalUpdateUiState() {
		LynxCpuState cpu = DebugApi.GetCpuState<LynxCpuState>(CpuType.Lynx);
		LynxPpuState ppu = DebugApi.GetPpuState<LynxPpuState>(CpuType.Lynx);

		UpdateCycleCount(cpu.CycleCount);

		RegA = cpu.A;
		RegX = cpu.X;
		RegY = cpu.Y;
		RegSP = cpu.SP;
		RegPC = cpu.PC;
		RegPS = cpu.PS;

		StringBuilder sb = new StringBuilder();
		for (UInt32 i = (UInt32)0x0100 + cpu.SP + 1; i < 0x0200; i++) {
			sb.Append($"${DebugApi.GetMemoryValue(MemoryType.LynxMemory, i):X2} ");
		}

		StackPreview = sb.ToString();

		Cycle = ppu.Cycle;
		Scanline = ppu.Scanline;
		FrameCount = ppu.FrameCount;
	}

	protected override void InternalUpdateConsoleState() {
		LynxCpuState cpu = DebugApi.GetCpuState<LynxCpuState>(CpuType.Lynx);

		cpu.A = RegA;
		cpu.X = RegX;
		cpu.Y = RegY;
		cpu.SP = RegSP;
		cpu.PC = RegPC;
		cpu.PS = RegPS;

		DebugApi.SetCpuState(cpu, CpuType.Lynx);
	}
}
