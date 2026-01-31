#pragma once
#include "pch.h"
#include "Shared/MemoryType.h"
#include "Shared/BaseState.h"
#include "Shared/ArmEnums.h"
#include "Utilities/Serializer.h"

/// <summary>
/// ARM7TDMI CPU operating modes.
/// Each mode has its own banked registers and privilege level.
/// </summary>
enum class GbaCpuMode : uint8_t {
	User = 0b10000,       ///< Normal execution, no privileged access
	Fiq = 0b10001,        ///< Fast interrupt (banked R8-R14)
	Irq = 0b10010,        ///< Standard interrupt (banked R13-R14)
	Supervisor = 0b10011, ///< SWI handler mode
	Abort = 0b10111,      ///< Memory fault handler
	Undefined = 0b11011,  ///< Undefined instruction handler
	System = 0b11111,     ///< Privileged mode using User registers
};

/// <summary>
/// ARM exception vector addresses in BIOS.
/// CPU jumps to these addresses when exceptions occur.
/// </summary>
enum class GbaCpuVector : uint32_t {
	Undefined = 0x04,      ///< Undefined instruction
	SoftwareIrq = 0x08,    ///< SWI instruction
	AbortPrefetch = 0x0C,  ///< Prefetch abort (bad instruction fetch)
	AbortData = 0x10,      ///< Data abort (bad data access)
	Irq = 0x18,            ///< Hardware interrupt
	Fiq = 0x1C,            ///< Fast interrupt
};

typedef uint8_t GbaAccessModeVal;

/// <summary>
/// Memory access mode flags for bus timing and behavior.
/// </summary>
namespace GbaAccessMode {
enum Mode {
	Sequential = (1 << 0),  ///< Sequential access (faster)
	Word = (1 << 1),        ///< 32-bit access
	HalfWord = (1 << 2),    ///< 16-bit access
	Byte = (1 << 3),        ///< 8-bit access
	Signed = (1 << 4),      ///< Sign-extend result
	NoRotate = (1 << 5),    ///< Don't rotate misaligned reads
	Prefetch = (1 << 6),    ///< Instruction prefetch
	Dma = (1 << 7)          ///< DMA transfer access
};
} // namespace GbaAccessMode

/// <summary>
/// ARM CPU status flags (CPSR bits).
/// </summary>
struct GbaCpuFlags {
	GbaCpuMode Mode;    ///< Current CPU mode
	bool Thumb;         ///< Thumb state (16-bit instructions)
	bool FiqDisable;    ///< FIQ masked
	bool IrqDisable;    ///< IRQ masked

	bool Overflow;      ///< Arithmetic overflow (V)
	bool Carry;         ///< Carry/borrow (C)
	bool Zero;          ///< Zero result (Z)
	bool Negative;      ///< Negative/sign (N)

	/// <summary>Packs flags into 32-bit CPSR format.</summary>
	uint32_t ToInt32() {
		return (
		    (Negative << 31) |
		    (Zero << 30) |
		    (Carry << 29) |
		    (Overflow << 28) |

		    (IrqDisable << 7) |
		    (FiqDisable << 6) |
		    (Thumb << 5) |
		    (uint8_t)Mode);
	}
};

/// <summary>
/// Single instruction in the CPU pipeline.
/// </summary>
struct GbaInstructionData {
	uint32_t Address;   ///< Instruction address
	uint32_t OpCode;    ///< Instruction opcode
};

/// <summary>
/// ARM7 3-stage pipeline state (Fetch, Decode, Execute).
/// </summary>
struct GbaCpuPipeline {
	GbaInstructionData Fetch;     ///< Instruction being fetched
	GbaInstructionData Decode;    ///< Instruction being decoded
	GbaInstructionData Execute;   ///< Instruction being executed
	bool ReloadRequested;         ///< Pipeline flush pending
	GbaAccessModeVal Mode;        ///< Current access mode
};

/// <summary>
/// Complete ARM7TDMI CPU state including banked registers.
/// </summary>
struct GbaCpuState : BaseState {
	GbaCpuPipeline Pipeline;
	uint32_t R[16];       ///< General purpose registers (R0-R15, R15=PC)
	GbaCpuFlags CPSR;     ///< Current Program Status Register
	bool Stopped;         ///< CPU halted (STOP instruction)
	bool Frozen;          ///< CPU frozen (debugging)

	// Banked registers for each mode
	uint32_t UserRegs[7];       ///< User/System R8-R14
	uint32_t FiqRegs[7];        ///< FIQ R8-R14 (fully banked)
	uint32_t IrqRegs[2];        ///< IRQ R13-R14

	uint32_t SupervisorRegs[2]; ///< Supervisor R13-R14
	uint32_t AbortRegs[2];      ///< Abort R13-R14
	uint32_t UndefinedRegs[2];  ///< Undefined R13-R14

	// Saved PSR for each exception mode
	GbaCpuFlags FiqSpsr;        ///< FIQ Saved Program Status Register
	GbaCpuFlags IrqSpsr;        ///< IRQ SPSR
	GbaCpuFlags SupervisorSpsr; ///< Supervisor SPSR
	GbaCpuFlags AbortSpsr;      ///< Abort SPSR
	GbaCpuFlags UndefinedSpsr;  ///< Undefined SPSR

	uint64_t CycleCount;  ///< Total CPU cycles executed
};

/// <summary>Stereo 3D mode for backgrounds (used by some homebrew).</summary>
enum class GbaBgStereoMode : uint8_t {
	Disabled,     ///< Normal display
	EvenColumns,  ///< Display on even columns only
	OddColumns,   ///< Display on odd columns only
	Both          ///< Display on all columns
};

/// <summary>
/// Background layer configuration state.
/// </summary>
struct GbaBgConfig {
	uint16_t Control;      ///< BGCNT register value
	uint16_t TilemapAddr;  ///< Screen base block address
	uint16_t TilesetAddr;  ///< Character base block address
	uint16_t ScrollX;      ///< Horizontal scroll offset
	uint16_t ScrollY;      ///< Vertical scroll offset
	uint8_t ScreenSize;    ///< Screen size (0-3)
	bool DoubleWidth;      ///< 512 pixel width
	bool DoubleHeight;     ///< 512 pixel height
	uint8_t Priority;      ///< Display priority (0=highest)
	bool Mosaic;           ///< Mosaic effect enabled
	bool WrapAround;       ///< Wrap at edges (affine only)
	bool Bpp8Mode;         ///< 8bpp tiles (256 colors)
	bool Enabled;          ///< Layer enabled
	uint8_t EnableTimer;   ///< Frames until enable takes effect
	uint8_t DisableTimer;  ///< Frames until disable takes effect
	GbaBgStereoMode StereoMode;  ///< Stereo 3D mode
};

/// <summary>
/// Affine transformation parameters for rotation/scaling BGs.
/// </summary>
struct GbaTransformConfig {
	uint32_t OriginX;        ///< Reference point X (fixed-point)
	uint32_t OriginY;        ///< Reference point Y

	int32_t LatchOriginX;    ///< Latched origin X (per frame)
	int32_t LatchOriginY;    ///< Latched origin Y

	int16_t Matrix[4];       ///< 2x2 transform matrix (PA, PB, PC, PD)

	bool PendingUpdateX;     ///< X origin write pending
	bool PendingUpdateY;     ///< Y origin write pending
	bool NeedInit;           ///< Needs initialization
};

/// <summary>
/// Window boundary configuration.
/// </summary>
struct GbaWindowConfig {
	uint8_t LeftX;     ///< Left edge (inclusive)
	uint8_t RightX;    ///< Right edge (exclusive)
	uint8_t TopY;      ///< Top edge (inclusive)
	uint8_t BottomY;   ///< Bottom edge (exclusive)
};

/// <summary>
/// PPU color blending effect type.
/// </summary>
enum class GbaPpuBlendEffect : uint8_t {
	None,                ///< No blending
	AlphaBlend,          ///< Semi-transparency (EVA*A + EVB*B)
	IncreaseBrightness,  ///< Fade to white
	DecreaseBrightness   ///< Fade to black
};

/// <summary>
/// Sprite (OBJ) rendering mode.
/// </summary>
enum class GbaPpuObjMode : uint8_t {
	Normal,        ///< Standard sprite
	Blending,      ///< Semi-transparent (always first target)
	Window,        ///< Defines OBJ window region
	Stereoscopic   ///< Prohibited in GBA (treated as double-size)
};

/// <summary>
/// PPU memory access type flags for bus conflict detection.
/// </summary>
namespace GbaPpuMemAccess {
enum GbaPpuMemAccess : uint8_t {
	None = 0,
	Palette = 1,   ///< Accessing palette RAM
	Vram = 2,      ///< Accessing VRAM (BG)
	Oam = 4,       ///< Accessing OAM
	VramObj = 8    ///< Accessing VRAM (OBJ)
};
} // namespace GbaPpuMemAccess

/// <summary>
/// Complete PPU register state.
/// </summary>
struct GbaPpuState : BaseState {
	uint32_t FrameCount;   ///< Frames rendered
	uint16_t Cycle;        ///< Current cycle within scanline (0-1231)
	uint16_t Scanline;     ///< Current scanline (0-227)

	// DISPCNT register ($4000000)
	uint8_t Control;
	uint8_t BgMode;                    ///< Background mode (0-5)
	bool DisplayFrameSelect;           ///< Frame buffer select (modes 4/5)
	bool AllowHblankOamAccess;         ///< OAM accessible during HBlank
	bool ObjVramMappingOneDimension;   ///< 1D sprite tile mapping
	bool ForcedBlank;                  ///< Display disabled (white)
	uint8_t ForcedBlankDisableTimer;   ///< Frames until blank ends
	bool StereoscopicEnabled;          ///< Stereo 3D mode enabled

	uint8_t Control2;
	uint8_t ObjEnableTimer;            ///< Frames until OBJ enable
	bool ObjLayerEnabled;              ///< Sprite layer enabled
	bool Window0Enabled;               ///< Window 0 active
	bool Window1Enabled;               ///< Window 1 active
	bool ObjWindowEnabled;             ///< Sprite window active

	// DISPSTAT register ($4000004)
	uint8_t DispStat;
	bool VblankIrqEnabled;             ///< VBlank triggers IRQ
	bool HblankIrqEnabled;             ///< HBlank triggers IRQ
	bool ScanlineIrqEnabled;           ///< V-count match triggers IRQ
	uint8_t Lyc;                       ///< Scanline compare value

	// Blending registers
	uint8_t BlendMainControl;
	bool BlendMain[6];                 ///< First target layers
	uint8_t BlendSubControl;
	bool BlendSub[6];                  ///< Second target layers
	GbaPpuBlendEffect BlendEffect;     ///< Current blend mode
	uint8_t BlendMainCoefficient;      ///< EVA (0-16)
	uint8_t BlendSubCoefficient;       ///< EVB (0-16)
	uint8_t Brightness;                ///< EVY for brightness (0-16)

	GbaBgConfig BgLayers[4];           ///< BG0-BG3 configuration
	GbaTransformConfig Transform[2];   ///< BG2/BG3 affine transforms
	GbaWindowConfig Window[2];         ///< Window 0/1 bounds

	// Mosaic settings
	uint8_t BgMosaicSizeX;             ///< BG mosaic horizontal size
	uint8_t BgMosaicSizeY;             ///< BG mosaic vertical size
	uint8_t ObjMosaicSizeX;            ///< OBJ mosaic horizontal size
	uint8_t ObjMosaicSizeY;            ///< OBJ mosaic vertical size

	// Window layer visibility
	uint8_t Window0Control;
	uint8_t Window1Control;
	uint8_t ObjWindowControl;
	uint8_t OutWindowControl;
	bool WindowActiveLayers[5][6];     ///< [window][layer] visibility
};

/// <summary>
/// Memory manager state including interrupts and wait states.
/// </summary>
struct GbaMemoryManagerState {
	uint16_t IE;         ///< Interrupt Enable ($4000200)
	uint16_t IF;         ///< Interrupt Flags ($4000202)
	uint16_t NewIE;      ///< Pending IE write
	uint16_t NewIF;      ///< Pending IF write

	uint16_t WaitControl;              ///< WAITCNT ($4000204)
	uint8_t PrgWaitStates0[2] = {5, 3}; ///< ROM wait states bank 0 [N, S]
	uint8_t PrgWaitStates1[2] = {5, 5}; ///< ROM wait states bank 1
	uint8_t PrgWaitStates2[2] = {5, 9}; ///< ROM wait states bank 2
	uint8_t SramWaitStates = 5;         ///< SRAM wait states
	bool PrefetchEnabled;               ///< Prefetch buffer enabled
	uint8_t IME;         ///< Interrupt Master Enable ($4000208)
	uint8_t NewIME;      ///< Pending IME write
	uint8_t IrqUpdateCounter;  ///< Cycles until IRQ check
	uint8_t IrqLine;           ///< Current IRQ line state
	uint8_t IrqPending;        ///< IRQ pending
	bool BusLocked;            ///< Bus locked by DMA
	bool StopMode;             ///< CPU in STOP mode
	bool PostBootFlag;         ///< Boot ROM completed

	// Open bus values for different regions
	uint8_t BootRomOpenBus[4];
	uint8_t InternalOpenBus[4];
	uint8_t IwramOpenBus[4];
};

/// <summary>
/// ROM prefetch buffer state for faster sequential reads.
/// </summary>
struct GbaRomPrefetchState {
	uint32_t ReadAddr;      ///< Address being read
	uint32_t PrefetchAddr;  ///< Next address to prefetch
	uint8_t ClockCounter;
	bool WasFilled;
	bool Started;
	bool Sequential;
	bool HitBoundary;
};

struct GbaTimerState {
	uint16_t ReloadValue;
	uint16_t NewReloadValue;
	uint16_t PrescaleMask;
	uint16_t Timer;
	uint8_t Control;

	uint8_t EnableDelay;
	bool WritePending;
	bool Mode;
	bool IrqEnabled;
	bool Enabled;
	bool ProcessTimer;
};

struct GbaTimersState {
	GbaTimerState Timer[4];
};

enum class GbaDmaTrigger : uint8_t {
	Immediate = 0,
	VBlank = 1,
	HBlank = 2,
	Special = 3
};

enum class GbaDmaAddrMode : uint8_t {
	Increment,
	Decrement,
	Fixed,
	IncrementReload
};

struct GbaDmaChannel {
	uint64_t StartClock;
	uint32_t ReadValue;

	uint32_t Destination;
	uint32_t Source;
	uint16_t Length;

	uint32_t DestLatch;
	uint32_t SrcLatch;
	uint16_t LenLatch;

	uint16_t Control;

	GbaDmaAddrMode DestMode;
	GbaDmaAddrMode SrcMode;

	bool Repeat;
	bool WordTransfer;
	bool DrqMode;

	GbaDmaTrigger Trigger;
	bool IrqEnabled;
	bool Enabled;
	bool Active;

	bool Pending;
};

struct GbaDmaControllerState {
	GbaDmaChannel Ch[4];
};

struct GbaSquareState {
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
	uint8_t DutyPos;
	uint8_t Output;
};

struct GbaNoiseState {
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

struct GbaWaveState {
	bool DacEnabled;
	bool DoubleLength;
	uint8_t SelectedBank;

	uint8_t SampleBuffer;
	uint8_t Ram[0x20];
	uint8_t Position;

	uint8_t Volume;
	bool OverrideVolume;
	uint16_t Frequency;

	uint16_t Length;
	bool LengthEnabled;

	bool Enabled;
	uint16_t Timer;
	uint8_t Output;
};

struct GbaApuState {
	int8_t DmaSampleA;
	int8_t DmaSampleB;

	uint8_t VolumeControl;
	uint8_t GbVolume;
	uint8_t VolumeA;
	uint8_t VolumeB;

	uint8_t DmaSoundControl;
	bool EnableRightA;
	bool EnableLeftA;
	uint8_t TimerA;
	bool EnableRightB;
	bool EnableLeftB;
	uint8_t TimerB;

	uint8_t EnabledGb;
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

	uint8_t FrameSequenceStep;

	bool ApuEnabled;

	uint16_t Bias;
	uint8_t SamplingRate;
};

struct GbaSerialState {
	uint64_t StartMasterClock;
	uint64_t EndMasterClock;
	uint64_t IrqMasterClock;

	uint16_t Data[4]; // 120-127

	uint16_t Control; // 128-129
	bool InternalShiftClock;
	bool InternalShiftClockSpeed2MHz;
	bool Active;
	bool TransferWord;
	bool IrqEnabled;

	uint16_t SendData;   // 12A-12B
	uint16_t Mode;       // 134-135
	uint16_t JoyControl; // 140-141
	uint32_t JoyReceive; // 150-153
	uint32_t JoySend;    // 154-157
	uint8_t JoyStatus;   // 158
};

struct GbaControlManagerState {
	uint16_t KeyControl;
	uint16_t ActiveKeys;
};

struct GbaApuDebugState {
	GbaApuState Common;
	GbaSquareState Square1;
	GbaSquareState Square2;
	GbaWaveState Wave;
	GbaNoiseState Noise;
};

struct GbaGpioState {
	uint8_t Data;
	uint8_t WritablePins;
	bool ReadWrite;
};

struct GbaCartState {
	bool HasGpio;
	GbaGpioState Gpio;
};

struct GbaState {
	GbaCpuState Cpu;
	GbaPpuState Ppu;
	GbaApuDebugState Apu;
	GbaMemoryManagerState MemoryManager;
	GbaDmaControllerState Dma;
	GbaTimersState Timer;
	GbaRomPrefetchState Prefetch;
	GbaControlManagerState ControlManager;
	GbaCartState Cart;
};

enum class GbaThumbOpCategory {
	MoveShiftedRegister,
	AddSubtract,
	MoveCmpAddSub,
	AluOperation,
	HiRegBranchExch,
	PcRelLoad,
	LoadStoreRegOffset,
	LoadStoreSignExtended,
	LoadStoreImmOffset,
	LoadStoreHalfWord,
	SpRelLoadStore,
	LoadAddress,
	AddOffsetToSp,
	PushPopReg,
	MultipleLoadStore,
	ConditionalBranch,
	SoftwareInterrupt,
	UnconditionalBranch,
	LongBranchLink,

	InvalidOp,
};

enum class GbaIrqSource {
	None = 0,
	LcdVblank = 1 << 0,
	LcdHblank = 1 << 1,
	LcdScanlineMatch = 1 << 2,

	Timer0 = 1 << 3,
	Timer1 = 1 << 4,
	Timer2 = 1 << 5,
	Timer3 = 1 << 6,

	Serial = 1 << 7,

	DmaChannel0 = 1 << 8,
	DmaChannel1 = 1 << 9,
	DmaChannel2 = 1 << 10,
	DmaChannel3 = 1 << 11,

	Keypad = 1 << 12,
	External = 1 << 13
};

class GbaConstants {
public:
	static constexpr uint32_t ScreenWidth = 240;
	static constexpr uint32_t ScreenHeight = 160;
	static constexpr uint32_t PixelCount = GbaConstants::ScreenWidth * GbaConstants::ScreenHeight;

	static constexpr uint32_t ScanlineCount = 228;
};