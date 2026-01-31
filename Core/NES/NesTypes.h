#pragma once
#include "pch.h"
#include "Shared/BaseState.h"


/// <summary>
/// 6502 processor status flag bits (P register).
/// </summary>
namespace PSFlags {
enum PSFlags : uint8_t {
	Carry = 0x01,      ///< Carry flag (C)
	Zero = 0x02,       ///< Zero flag (Z)
	Interrupt = 0x04,  ///< Interrupt disable (I)
	Decimal = 0x08,    ///< Decimal mode (D, unused on NES)
	Break = 0x10,      ///< Break command (B)
	Reserved = 0x20,   ///< Always set (unused)
	Overflow = 0x40,   ///< Overflow flag (V)
	Negative = 0x80    ///< Negative flag (N)
};
} // namespace PSFlags


/// <summary>
/// 6502 addressing modes for instruction decoding.
/// </summary>
enum class NesAddrMode {
	None,    ///< Not used
	Acc,     ///< Accumulator
	Imp,     ///< Implied
	Imm,     ///< Immediate
	Rel,     ///< Relative (branch)
	Zero,    ///< Zero page
	Abs,     ///< Absolute
	ZeroX,   ///< Zero page,X
	ZeroY,   ///< Zero page,Y
	Ind,     ///< Indirect (JMP)
	IndX,    ///< (Indirect,X)
	IndY,    ///< (Indirect),Y
	IndYW,   ///< (Indirect),Y with wrap
	AbsX,    ///< Absolute,X
	AbsXW,   ///< Absolute,X with wrap
	AbsY,    ///< Absolute,Y
	AbsYW,   ///< Absolute,Y with wrap
	Other    ///< Special/illegal
};


/// <summary>
/// Sources of IRQ (interrupt requests) on the NES.
/// </summary>
enum class IRQSource {
	External = 1,      ///< External IRQ (mapper, controller, etc.)
	FrameCounter = 2,  ///< APU frame counter
	DMC = 4,           ///< DMC sample playback
	FdsDisk = 8,       ///< FDS disk system
	Epsm = 16          ///< EPSM expansion audio
};


/// <summary>
/// Memory operation type for tracing and debugging.
/// </summary>
enum class MemoryOperation {
	Read = 1,   ///< Read operation
	Write = 2,  ///< Write operation
	Any = 3     ///< Any access
};


/// <summary>
/// Complete 6502 CPU state for NES emulation.
/// </summary>
struct NesCpuState : BaseState {
	uint64_t CycleCount = 0; ///< Total CPU cycles executed
	uint16_t PC = 0;         ///< Program counter
	uint8_t SP = 0;          ///< Stack pointer
	uint8_t A = 0;           ///< Accumulator
	uint8_t X = 0;           ///< X index register
	uint8_t Y = 0;           ///< Y index register
	uint8_t PS = 0;          ///< Processor status (flags)
	uint8_t IrqFlag = 0;     ///< IRQ pending flag
	bool NmiFlag = false;    ///< NMI pending flag
};


/// <summary>
/// Types of PRG (program) memory in NES address space.
/// </summary>
enum class PrgMemoryType {
	PrgRom,     ///< Cartridge ROM
	SaveRam,    ///< Battery-backed RAM
	WorkRam,    ///< Internal work RAM
	MapperRam,  ///< Mapper-controlled RAM
};


/// <summary>
/// Types of CHR (character/graphics) memory in NES PPU address space.
/// </summary>
enum class ChrMemoryType {
	Default,      ///< Default (mapper-specific)
	ChrRom,       ///< CHR ROM (cartridge)
	ChrRam,       ///< CHR RAM (cartridge)
	NametableRam, ///< Nametable RAM (CIRAM)
	MapperRam,    ///< Mapper-controlled RAM
};


/// <summary>
/// Memory access permissions for address decoding.
/// </summary>
enum MemoryAccessType {
	Unspecified = -1, ///< Not specified
	NoAccess = 0x00,  ///< No access
	Read = 0x01,      ///< Read allowed
	Write = 0x02,     ///< Write allowed
	ReadWrite = 0x03  ///< Read and write allowed
};


/// <summary>
/// Nametable mirroring types for PPU address mapping.
/// </summary>
enum class MirroringType {
	Horizontal,   ///< Horizontal mirroring (vertical split)
	Vertical,     ///< Vertical mirroring (horizontal split)
	ScreenAOnly,  ///< Single screen A
	ScreenBOnly,  ///< Single screen B
	FourScreens   ///< Four-screen VRAM
};


/// <summary>
/// Value types for storing mapper state variables.
/// </summary>
enum class MapperStateValueType {
	None,      ///< No value
	String,    ///< String value
	Bool,      ///< Boolean value
	Number8,   ///< 8-bit integer
	Number16,  ///< 16-bit integer
	Number32   ///< 32-bit integer
};

/// <summary>
/// Entry for saving/restoring mapper state (address, name, value).
/// </summary>
struct MapperStateEntry {
	static constexpr int MaxLength = 40;

	int64_t RawValue = INT64_MIN; ///< Raw value (if numeric)
	MapperStateValueType Type = MapperStateValueType::Number8; ///< Value type
	uint8_t Address[MapperStateEntry::MaxLength] = {}; ///< Address string
	uint8_t Name[MapperStateEntry::MaxLength] = {};    ///< Name string
	uint8_t Value[MapperStateEntry::MaxLength] = {};   ///< Value string

	MapperStateEntry() {}

	MapperStateEntry(string address, string name) {
		memcpy(Address, address.c_str(), std::min<size_t>(MapperStateEntry::MaxLength - 1, address.size()));
		memcpy(Name, name.c_str(), std::min<size_t>(MapperStateEntry::MaxLength - 1, name.size()));
		Type = MapperStateValueType::None;
	}

	MapperStateEntry(string address, string name, string value, int64_t rawValue = INT64_MIN) : MapperStateEntry(address, name) {
		memcpy(Value, value.c_str(), std::min<size_t>(MapperStateEntry::MaxLength - 1, value.size()));
		RawValue = rawValue;
		Type = MapperStateValueType::String;
	}

	MapperStateEntry(string address, string name, bool value) : MapperStateEntry(address, name) {
		Value[0] = value;
		Type = MapperStateValueType::Bool;
	}

	MapperStateEntry(string address, string name, int64_t value, MapperStateValueType length) : MapperStateEntry(address, name) {
		for (int i = 0; i < 8; i++) {
			Value[i] = value & 0xFF;
			value >>= 8;
		}
		Type = length;
	}
};

struct CartridgeState {
	uint32_t PrgRomSize = 0;
	uint32_t ChrRomSize = 0;
	uint32_t ChrRamSize = 0;

	uint32_t PrgPageCount = 0;
	uint32_t PrgPageSize = 0;
	int32_t PrgMemoryOffset[0x100] = {};
	PrgMemoryType PrgType[0x100] = {};
	MemoryAccessType PrgMemoryAccess[0x100] = {};

	uint32_t ChrPageCount = 0;
	uint32_t ChrPageSize = 0;
	uint32_t ChrRamPageSize = 0;
	int32_t ChrMemoryOffset[0x40] = {};
	ChrMemoryType ChrType[0x40] = {};
	MemoryAccessType ChrMemoryAccess[0x40] = {};

	uint32_t WorkRamPageSize = 0;
	uint32_t SaveRamPageSize = 0;

	MirroringType Mirroring = {};
	bool HasBattery = false;

	uint32_t CustomEntryCount = 0;
	MapperStateEntry CustomEntries[200] = {};
};

struct PPUStatusFlags {
	bool SpriteOverflow;
	bool Sprite0Hit;
	bool VerticalBlank;
};

struct PpuControlFlags {
	uint16_t BackgroundPatternAddr;
	uint16_t SpritePatternAddr;
	bool VerticalWrite;
	bool LargeSprites;
	bool SecondaryPpu;
	bool NmiOnVerticalBlank;
};

struct PpuMaskFlags {
	bool Grayscale;
	bool BackgroundMask;
	bool SpriteMask;
	bool BackgroundEnabled;
	bool SpritesEnabled;
	bool IntensifyRed;
	bool IntensifyGreen;
	bool IntensifyBlue;
};

struct TileInfo {
	uint16_t TileAddr;
	uint8_t LowByte;
	uint8_t HighByte;
	uint8_t PaletteOffset;
};

struct NesSpriteInfo {
	bool HorizontalMirror;
	bool BackgroundPriority;
	uint8_t SpriteX;
	uint8_t LowByte;
	uint8_t HighByte;
	uint8_t PaletteOffset;
};

struct NesPpuState : public BaseState {
	PPUStatusFlags StatusFlags;
	PpuMaskFlags Mask;
	PpuControlFlags Control;

	int32_t Scanline;
	uint32_t Cycle;
	uint32_t FrameCount;
	uint32_t NmiScanline;
	uint32_t ScanlineCount;
	uint32_t SafeOamScanline;
	uint16_t BusAddress;
	uint8_t MemoryReadBuffer;

	uint16_t VideoRamAddr;
	uint16_t TmpVideoRamAddr;
	uint8_t ScrollX;
	bool WriteToggle;
	uint8_t SpriteRamAddr;
};

struct ApuLengthCounterState {
	bool Halt;
	uint8_t Counter;
	uint8_t ReloadValue;
};

struct ApuEnvelopeState {
	bool StartFlag;
	bool Loop;
	bool ConstantVolume;
	uint8_t Divider;
	uint8_t Counter;
	uint8_t Volume;
};

struct ApuSquareState {
	uint8_t Duty;
	uint8_t DutyPosition;
	uint16_t Period;
	uint16_t Timer;

	bool SweepEnabled;
	bool SweepNegate;
	uint8_t SweepPeriod;
	uint8_t SweepShift;

	bool Enabled;
	uint8_t OutputVolume;
	double Frequency;

	ApuLengthCounterState LengthCounter;
	ApuEnvelopeState Envelope;
};

struct ApuTriangleState {
	uint16_t Period;
	uint16_t Timer;
	uint8_t SequencePosition;

	bool Enabled;
	double Frequency;
	uint8_t OutputVolume;

	uint8_t LinearCounter;
	uint8_t LinearCounterReload;
	bool LinearReloadFlag;

	ApuLengthCounterState LengthCounter;
};

struct ApuNoiseState {
	uint16_t Period;
	uint16_t Timer;
	uint16_t ShiftRegister;
	bool ModeFlag;

	bool Enabled;
	double Frequency;
	uint8_t OutputVolume;

	ApuLengthCounterState LengthCounter;
	ApuEnvelopeState Envelope;
};

struct ApuDmcState {
	double SampleRate;
	uint16_t SampleAddr;
	uint16_t NextSampleAddr;
	uint16_t SampleLength;

	bool Loop;
	bool IrqEnabled;
	uint16_t Period;
	uint16_t Timer;
	uint16_t BytesRemaining;

	uint8_t OutputVolume;
};

struct ApuFrameCounterState {
	bool FiveStepMode;
	uint8_t SequencePosition;
	bool IrqEnabled;
};

struct ApuState {
	ApuSquareState Square1;
	ApuSquareState Square2;
	ApuTriangleState Triangle;
	ApuNoiseState Noise;
	ApuDmcState Dmc;
	ApuFrameCounterState FrameCounter;
};

enum class GameSystem {
	NesNtsc,
	NesPal,
	Famicom,
	Dendy,
	VsSystem,
	Playchoice,
	FDS,
	FamicomNetworkSystem,
	Unknown,
};

enum class BusConflictType {
	Default = 0,
	Yes,
	No
};

struct HashInfo {
	uint32_t Crc32 = 0;
	uint32_t PrgCrc32 = 0;
	uint32_t PrgChrCrc32 = 0;
};

enum class VsSystemType {
	Default = 0,
	RbiBaseballProtection = 1,
	TkoBoxingProtection = 2,
	SuperXeviousProtection = 3,
	IceClimberProtection = 4,
	VsDualSystem = 5,
	RaidOnBungelingBayProtection = 6,
};

enum class GameInputType {
	Unspecified = 0,
	StandardControllers = 1,
	FourScore = 2,
	FourPlayerAdapter = 3,
	VsSystem = 4,
	VsSystemSwapped = 5,
	VsSystemSwapAB = 6,
	VsZapper = 7,
	Zapper = 8,
	TwoZappers = 9,
	BandaiHypershot = 0x0A,
	PowerPadSideA = 0x0B,
	PowerPadSideB = 0x0C,
	FamilyTrainerSideA = 0x0D,
	FamilyTrainerSideB = 0x0E,
	ArkanoidControllerNes = 0x0F,
	ArkanoidControllerFamicom = 0x10,
	DoubleArkanoidController = 0x11,
	KonamiHyperShot = 0x12,
	PachinkoController = 0x13,
	ExcitingBoxing = 0x14,
	JissenMahjong = 0x15,
	PartyTap = 0x16,
	OekaKidsTablet = 0x17,
	BarcodeBattler = 0x18,
	MiraclePiano = 0x19,      // not supported yet
	PokkunMoguraa = 0x1A,     // not supported yet
	TopRider = 0x1B,          // not supported yet
	DoubleFisted = 0x1C,      // not supported yet
	Famicom3dSystem = 0x1D,   // not supported yet
	DoremikkoKeyboard = 0x1E, // not supported yet
	ROB = 0x1F,               // not supported yet
	FamicomDataRecorder = 0x20,
	TurboFile = 0x21,
	BattleBox = 0x22,
	FamilyBasicKeyboard = 0x23,
	Pec586Keyboard = 0x24, // not supported yet
	Bit79Keyboard = 0x25,  // not supported yet
	SuborKeyboard = 0x26,
	SuborKeyboardMouse1 = 0x27,
	SuborKeyboardMouse2 = 0x28,
	SnesMouse = 0x29,
	GenericMulticart = 0x2A, // not supported yet
	SnesControllers = 0x2B,
	RacermateBicycle = 0x2C, // not supported yet
	UForce = 0x2D,           // not supported yet
	LastEntry
};

enum class PpuModel {
	Ppu2C02 = 0,
	Ppu2C03 = 1,
	Ppu2C04A = 2,
	Ppu2C04B = 3,
	Ppu2C04C = 4,
	Ppu2C04D = 5,
	Ppu2C05A = 6,
	Ppu2C05B = 7,
	Ppu2C05C = 8,
	Ppu2C05D = 9,
	Ppu2C05E = 10
};

enum class AudioChannel {
	Square1 = 0,
	Square2 = 1,
	Triangle = 2,
	Noise = 3,
	DMC = 4,
	FDS = 5,
	MMC5 = 6,
	VRC6 = 7,
	VRC7 = 8,
	Namco163 = 9,
	Sunsoft5B = 10
};

struct NesState {
	NesCpuState Cpu;
	NesPpuState Ppu;
	CartridgeState Cartridge;
	ApuState Apu;
	uint32_t ClockRate;
};
