#pragma once
#include "pch.h"
#include "Shared/MemoryType.h"
#include "Shared/BaseState.h"


/// <summary>
/// Complete Game Boy CPU state (Sharp LR35902, Z80-like).
/// </summary>
struct GbCpuState : BaseState {
	uint64_t CycleCount;   ///< Total CPU cycles executed
	uint16_t PC;           ///< Program counter
	uint16_t SP;           ///< Stack pointer
	uint16_t HaltCounter;  ///< Cycles remaining in HALT

	uint8_t A;             ///< Accumulator
	uint8_t Flags;         ///< Flags register (Z, N, H, C)

	uint8_t B;
	uint8_t C;
	uint8_t D;
	uint8_t E;

	uint8_t H;
	uint8_t L;

	bool EiPending;        ///< EI instruction pending
	bool IME;              ///< Interrupt master enable
	bool HaltBug;          ///< HALT bug active
	bool Stopped;          ///< STOP instruction active
};


/// <summary>
/// Game Boy CPU flag bits (F register).
/// </summary>
namespace GbCpuFlags {
enum GbCpuFlags {
	Zero = 0x80,      ///< Zero flag (Z)
	AddSub = 0x40,    ///< Add/Subtract flag (N)
	HalfCarry = 0x20, ///< Half-carry flag (H)
	Carry = 0x10      ///< Carry flag (C)
};
} // namespace GbCpuFlags


/// <summary>
/// Game Boy interrupt sources (IF/IE bits).
/// </summary>
namespace GbIrqSource {
enum GbIrqSource {
	VerticalBlank = 0x01, ///< V-Blank interrupt
	LcdStat = 0x02,       ///< LCD STAT interrupt
	Timer = 0x04,         ///< Timer overflow
	Serial = 0x08,        ///< Serial transfer
	Joypad = 0x10         ///< Joypad input
};
} // namespace GbIrqSource


/// <summary>
/// Helper for 16-bit register access from two 8-bit halves.
/// </summary>
class Register16 {
	uint8_t* _low;
	uint8_t* _high;

public:
	Register16(uint8_t* high, uint8_t* low) {
		_high = high;
		_low = low;
	}

	uint16_t Read() {
		return (*_high << 8) | *_low;
	}

	void Write(uint16_t value) {
		*_high = (uint8_t)(value >> 8);
		*_low = (uint8_t)value;
	}

	void Inc() {
		Write(Read() + 1);
	}

	void Dec() {
		Write(Read() - 1);
	}

	operator uint16_t() { return Read(); }
};


/// <summary>
/// Game Boy PPU mode (STAT register bits 0-1).
/// </summary>
enum class PpuMode {
	HBlank,         ///< Mode 0: HBlank
	VBlank,         ///< Mode 1: VBlank
	OamEvaluation,  ///< Mode 2: OAM scan
	Drawing,        ///< Mode 3: Pixel transfer
	NoIrq,          ///< Not generating IRQ
};


/// <summary>
/// Types of OAM corruption (hardware bugs).
/// </summary>
enum class GbOamCorruptionType {
	Read,        ///< Corruption on OAM read
	Write,       ///< Corruption on OAM write
	ReadIncDec   ///< Corruption on OAM inc/dec
};


/// <summary>
/// LCD STAT interrupt sources (STAT register bits 3-6).
/// </summary>
namespace GbPpuStatusFlags {
enum GbPpuStatusFlags {
	CoincidenceIrq = 0x40, ///< LYC=LY coincidence
	OamIrq = 0x20,         ///< OAM interrupt
	VBlankIrq = 0x10,      ///< VBlank interrupt
	HBlankIrq = 0x08       ///< HBlank interrupt
};
} // namespace GbPpuStatusFlags


/// <summary>
/// Debug event color codes for PPU visualization.
/// </summary>
enum class EvtColor {
	HBlank = 0,
	VBlank = 1,
	OamEvaluation = 2,
	RenderingIdle = 3,
	RenderingBgLoad = 4,
	RenderingOamLoad = 5,
};


/// <summary>
/// Pixel type for PPU output (BG or OBJ).
/// </summary>
enum class GbPixelType : uint8_t {
	Background, ///< Background pixel
	Object      ///< Sprite/object pixel
};


/// <summary>
/// FIFO entry for pixel pipeline (color, attributes, index).
/// </summary>
struct GbFifoEntry {
	uint8_t Color;      ///< Palette color index
	uint8_t Attributes; ///< Attribute bits (priority, palette)
	uint8_t Index;      ///< Pixel index in tile
};

struct GbPpuFifo {
	uint8_t Position = 0;
	uint8_t Size = 0;
	GbFifoEntry Content[8] = {};

	void Reset() {
		Size = 0;
		Position = 0;
		memset(Content, 0, sizeof(Content));
	}

	void Pop() {
		Content[Position].Color = 0;
		Position = (Position + 1) & 0x07;
		Size--;
	}
};

struct GbPpuFetcher {
	uint16_t Addr = 0;
	uint8_t Attributes = 0;
	uint8_t Step = 0;
	uint8_t LowByte = 0;
	uint8_t HighByte = 0;
};

struct GbPpuState : public BaseState {
	uint8_t Scanline;
	uint16_t Cycle;
	uint16_t IdleCycles;
	PpuMode Mode;
	PpuMode IrqMode;
	bool StatIrqFlag;

	uint8_t Ly;
	int16_t LyForCompare;

	uint8_t LyCompare;
	bool LyCoincidenceFlag;
	uint8_t BgPalette;
	uint8_t ObjPalette0;
	uint8_t ObjPalette1;
	uint8_t ScrollX;
	uint8_t ScrollY;
	uint8_t WindowX;
	uint8_t WindowY;

	uint8_t Control;
	bool LcdEnabled;
	bool WindowTilemapSelect;
	bool WindowEnabled;
	bool BgTileSelect;
	bool BgTilemapSelect;
	bool LargeSprites;
	bool SpritesEnabled;
	bool BgEnabled;

	uint8_t Status;
	uint32_t FrameCount;

	bool CgbEnabled;
	uint8_t CgbVramBank;

	uint8_t CgbBgPalPosition;
	bool CgbBgPalAutoInc;
	uint16_t CgbBgPalettes[4 * 8];

	uint8_t CgbObjPalPosition;
	bool CgbObjPalAutoInc;
	uint16_t CgbObjPalettes[4 * 8];
};

struct GbDmaControllerState {
	uint8_t OamDmaSource;
	uint8_t DmaStartDelay;
	uint8_t InternalDest;
	uint8_t DmaCounter;
	uint8_t DmaReadBuffer;
	bool OamDmaRunning;

	uint16_t CgbDmaSource;
	uint16_t CgbDmaDest;
	uint8_t CgbDmaLength;
	bool CgbHdmaRunning;
	bool CgbHdmaPending;
	bool CgbHdmaTrigger;
};

struct GbTimerState {
	uint16_t Divider;

	bool NeedReload; // Set after TIMA (_counter) overflowed, next cycle will reload TMA into TIMA
	bool Reloaded;   // Set during the cycle on which TIMA is reloaded (affects TMA/TIMA writes)
	uint8_t Counter;
	uint8_t Modulo;

	uint8_t Control;
	bool TimerEnabled;
	uint16_t TimerDivider;
};

struct GbSquareState {
	uint16_t Frequency;
	uint16_t Timer;

	uint16_t SweepTimer;
	uint16_t SweepFreq;
	uint16_t SweepPeriod;
	uint8_t SweepUpdateDelay;
	bool SweepNegate;
	uint8_t SweepShift;

	bool SweepEnabled;
	bool SweepNegateCalcDone;

	uint8_t Volume;
	uint8_t EnvVolume;
	bool EnvRaiseVolume;
	uint8_t EnvPeriod;
	uint8_t EnvTimer;
	bool EnvStopped;

	uint8_t Duty;

	uint8_t Length;
	bool LengthEnabled;

	bool Enabled;
	bool FirstStep;
	uint8_t DutyPos;
	uint8_t Output;
};

struct GbNoiseState {
	uint8_t Volume;
	uint8_t EnvVolume;
	bool EnvRaiseVolume;
	uint8_t EnvPeriod;
	uint8_t EnvTimer;
	bool EnvStopped;

	uint8_t Length;
	bool LengthEnabled;

	uint16_t ShiftRegister;

	uint8_t PeriodShift;
	uint8_t Divisor;
	bool ShortWidthMode;

	bool Enabled;
	uint32_t Timer;
	uint8_t Output;
};

struct GbWaveState {
	bool DacEnabled;

	uint8_t SampleBuffer;
	uint8_t Ram[0x10];
	uint8_t Position;

	uint8_t Volume;
	uint16_t Frequency;

	uint16_t Length;
	bool LengthEnabled;

	bool Enabled;
	uint16_t Timer;
	uint8_t Output;
};

struct GbApuState {
	bool ApuEnabled;

	uint8_t EnableLeftSq1;
	uint8_t EnableLeftSq2;
	uint8_t EnableLeftWave;
	uint8_t EnableLeftNoise;

	uint8_t EnableRightSq1;
	uint8_t EnableRightSq2;
	uint8_t EnableRightWave;
	uint8_t EnableRightNoise;

	uint8_t LeftVolume;
	uint8_t RightVolume;

	bool ExtAudioLeftEnabled;
	bool ExtAudioRightEnabled;

	uint8_t FrameSequenceStep;
};

struct GbApuDebugState {
	GbApuState Common;
	GbSquareState Square1;
	GbSquareState Square2;
	GbWaveState Wave;
	GbNoiseState Noise;
};

enum class RegisterAccess {
	None = 0,
	Read = 1,
	Write = 2,
	ReadWrite = 3
};

enum class GbMemoryType {
	None = 0,
	PrgRom = (int)MemoryType::GbPrgRom,
	WorkRam = (int)MemoryType::GbWorkRam,
	CartRam = (int)MemoryType::GbCartRam,
	BootRom = (int)MemoryType::GbBootRom,
};

struct GbMemoryManagerState {
	uint64_t ApuCycleCount;

	uint8_t CgbWorkRamBank;
	bool CgbSwitchSpeedRequest;
	bool CgbHighSpeed;

	uint8_t CgbRegRpInfrared;

	uint8_t CgbRegFF72;
	uint8_t CgbRegFF73;
	uint8_t CgbRegFF74;
	uint8_t CgbRegFF75;

	bool DisableBootRom;
	uint8_t IrqRequests;
	uint8_t IrqEnabled;

	uint8_t SerialData;
	uint8_t SerialControl;
	uint8_t SerialBitCount;

	bool IsReadRegister[0x100];
	bool IsWriteRegister[0x100];

	GbMemoryType MemoryType[0x100];
	uint32_t MemoryOffset[0x100];
	RegisterAccess MemoryAccessType[0x100];
};

struct GbControlManagerState {
	uint8_t InputSelect;
};

enum class GbType {
	Gb = 0,
	Cgb = 1,
};

struct GbState {
	GbType Type;
	GbCpuState Cpu;
	GbPpuState Ppu;
	GbApuDebugState Apu;
	GbMemoryManagerState MemoryManager;
	GbTimerState Timer;
	GbDmaControllerState Dma;
	GbControlManagerState ControlManager;
	bool HasBattery;
};
