#pragma once
#include "pch.h"
#include "Shared/BaseState.h"
#include "Debugger/DebugTypes.h"

/// <summary>
/// Atari 2600 6507 addressing modes.
/// The 6507 is a 6502 with only 13 address lines (8KB address space).
/// Same instruction set as 6502 but limited address range.
/// </summary>
enum class Atari2600AddrMode : uint8_t {
	None,   ///< Not used / illegal
	Acc,    ///< Accumulator (A)
	Imp,    ///< Implied
	Imm,    ///< Immediate (#$nn)
	Rel,    ///< Relative (branch target)
	Zero,   ///< Zero page ($nn)
	Abs,    ///< Absolute ($nnnn)
	ZeroX,  ///< Zero page,X ($nn,X)
	ZeroY,  ///< Zero page,Y ($nn,Y)
	Ind,    ///< Indirect (JMP ($nnnn))
	IndX,   ///< (Indirect,X) (($nn,X))
	IndY,   ///< (Indirect),Y (($nn),Y)
	IndYW,  ///< (Indirect),Y with extra cycle on page cross
	AbsX,   ///< Absolute,X ($nnnn,X)
	AbsXW,  ///< Absolute,X with extra cycle on page cross
	AbsY,   ///< Absolute,Y ($nnnn,Y)
	AbsYW   ///< Absolute,Y with extra cycle on page cross
};

/// <summary>
/// Atari 2600 6507 CPU register state.
/// The 6507 is a cut-down 6502 with 13 address pins (A0-A12)
/// giving a 8KB ($0000-$1FFF) address space.
/// </summary>
struct Atari2600CpuState : BaseState {
	uint64_t CycleCount = 0; ///< Total CPU cycles executed
	uint16_t PC = 0;         ///< Program counter (only low 13 bits used on bus)
	uint8_t SP = 0;          ///< Stack pointer
	uint8_t A = 0;           ///< Accumulator
	uint8_t X = 0;           ///< X index register
	uint8_t Y = 0;           ///< Y index register
	uint8_t PS = 0;          ///< Processor status (N V - B D I Z C)
	uint8_t IRQFlag = 0;     ///< IRQ pending flag (6507 has no IRQ pin, but BRK sets this)
	bool NmiFlag = false;    ///< NMI pending flag (6507 has no NMI pin — always false)
};
