#include "pch.h"
#include "PCE/PceTypes.h"

//=============================================================================
// PCE CPU Flag Operations
//=============================================================================

/// <summary>
/// Tests for PCE HuC6280 CPU flag operations.
/// The PCE CPU uses the same 6502-family flag layout as the NES.
/// </summary>
class PceCpuTypesTest : public ::testing::Test {
protected:
	PceCpuState _state{};

	void SetUp() override {
		_state = {};
		_state.PS = 0;
	}

	void ClearFlags(uint8_t flags) { _state.PS &= ~flags; }
	void SetFlags(uint8_t flags) { _state.PS |= flags; }
	bool CheckFlag(uint8_t flag) { return (_state.PS & flag) == flag; }
};

TEST_F(PceCpuTypesTest, InitialState_AllClear) {
	EXPECT_EQ(_state.PS, 0);
	EXPECT_EQ(_state.A, 0);
	EXPECT_EQ(_state.X, 0);
	EXPECT_EQ(_state.Y, 0);
	EXPECT_EQ(_state.SP, 0);
	EXPECT_EQ(_state.PC, 0);
}

TEST_F(PceCpuTypesTest, FlagLayout_MatchesMos6502) {
	EXPECT_EQ(PceCpuFlags::Carry, 0x01);
	EXPECT_EQ(PceCpuFlags::Zero, 0x02);
	EXPECT_EQ(PceCpuFlags::Interrupt, 0x04);
	EXPECT_EQ(PceCpuFlags::Decimal, 0x08);
	EXPECT_EQ(PceCpuFlags::Break, 0x10);
	EXPECT_EQ(PceCpuFlags::Memory, 0x20);
	EXPECT_EQ(PceCpuFlags::Overflow, 0x40);
	EXPECT_EQ(PceCpuFlags::Negative, 0x80);
}

TEST_F(PceCpuTypesTest, SetFlag_SetsOnlyTargetFlag) {
	SetFlags(PceCpuFlags::Zero);
	EXPECT_TRUE(CheckFlag(PceCpuFlags::Zero));
	EXPECT_FALSE(CheckFlag(PceCpuFlags::Carry));
	EXPECT_FALSE(CheckFlag(PceCpuFlags::Negative));
}

TEST_F(PceCpuTypesTest, ClearFlag_ClearsOnlyTargetFlag) {
	_state.PS = 0xFF;
	ClearFlags(PceCpuFlags::Carry);
	EXPECT_FALSE(CheckFlag(PceCpuFlags::Carry));
	EXPECT_TRUE(CheckFlag(PceCpuFlags::Zero));
	EXPECT_TRUE(CheckFlag(PceCpuFlags::Negative));
}

//=============================================================================
// Before/After Comparison: Branching vs Branchless SetZeroNegativeFlags
//=============================================================================

/// <summary>
/// PCE SetZeroNegativeFlags comparison tests.
/// The callers always clear Zero|Negative before calling, so
/// SetZeroNegativeFlags only needs to SET flags (never clear).
/// Both old and new implementations are additive-only (OR-based).
/// PCE Negative = 0x80 maps directly to bit 7 of value.
/// </summary>
class PceCpuBranchlessComparisonTest : public ::testing::Test {
protected:
	/// Old implementation: branching if/else if
	static uint8_t SetZeroNeg_Branching(uint8_t ps, uint8_t value) {
		// Caller has already cleared Zero|Negative
		if (value == 0) {
			ps |= PceCpuFlags::Zero;
		} else if (value & 0x80) {
			ps |= PceCpuFlags::Negative;
		}
		return ps;
	}

	/// New implementation: branchless (matches current PceCpu.cpp)
	static uint8_t SetZeroNeg_Branchless(uint8_t ps, uint8_t value) {
		// Caller has already cleared Zero|Negative
		ps |= (value == 0) ? PceCpuFlags::Zero : 0;
		ps |= (value & PceCpuFlags::Negative);
		return ps;
	}
};

TEST_F(PceCpuBranchlessComparisonTest, Exhaustive_All256Values_AllPSStates) {
	// Test every value (0-255) with multiple initial PS register states.
	// Note: callers always clear Zero|Negative before calling, so we
	// test with those flags already cleared in initial PS.
	uint8_t psStates[] = {
		0x00,                                           // All flags clear
		PceCpuFlags::Carry,                             // Only carry
		PceCpuFlags::Interrupt | PceCpuFlags::Decimal,  // I+D
		PceCpuFlags::Carry | PceCpuFlags::Overflow,     // C+V
		0x6D,                                           // Several flags
	};

	for (uint8_t initialPS : psStates) {
		// Simulate what callers do: clear Zero|Negative first
		uint8_t clearedPS = initialPS & ~(PceCpuFlags::Zero | PceCpuFlags::Negative);

		for (int v = 0; v <= 255; v++) {
			uint8_t value = static_cast<uint8_t>(v);
			uint8_t oldResult = SetZeroNeg_Branching(clearedPS, value);
			uint8_t newResult = SetZeroNeg_Branchless(clearedPS, value);

			EXPECT_EQ(oldResult, newResult)
				<< "PS mismatch for initialPS=0x" << std::hex << (int)initialPS
				<< " (cleared=0x" << (int)clearedPS << ")"
				<< " value=0x" << (int)value
				<< " old=0x" << (int)oldResult
				<< " new=0x" << (int)newResult;
		}
	}
}

TEST_F(PceCpuBranchlessComparisonTest, ZeroValue_SetsZeroFlag) {
	uint8_t ps = 0x00;
	uint8_t result = SetZeroNeg_Branchless(ps, 0x00);
	EXPECT_TRUE(result & PceCpuFlags::Zero);
	EXPECT_FALSE(result & PceCpuFlags::Negative);
}

TEST_F(PceCpuBranchlessComparisonTest, NegativeValue_SetsNegativeFlag) {
	uint8_t ps = 0x00;
	uint8_t result = SetZeroNeg_Branchless(ps, 0x80);
	EXPECT_FALSE(result & PceCpuFlags::Zero);
	EXPECT_TRUE(result & PceCpuFlags::Negative);
}

TEST_F(PceCpuBranchlessComparisonTest, PositiveNonZero_SetsNoFlags) {
	uint8_t ps = 0x00;
	uint8_t result = SetZeroNeg_Branchless(ps, 0x42);
	EXPECT_FALSE(result & PceCpuFlags::Zero);
	EXPECT_FALSE(result & PceCpuFlags::Negative);
}

TEST_F(PceCpuBranchlessComparisonTest, PreservesOtherFlags) {
	uint8_t ps = PceCpuFlags::Carry | PceCpuFlags::Overflow;
	uint8_t result = SetZeroNeg_Branchless(ps, 0x42);
	EXPECT_TRUE(result & PceCpuFlags::Carry);
	EXPECT_TRUE(result & PceCpuFlags::Overflow);
}

//=============================================================================
// Before/After Comparison: PCE CMP (Compare) Instruction
//=============================================================================

/// <summary>
/// PCE CMP comparison tests (identical logic to NES CMP, same ISA family).
/// Currently still branching in PceCpu.Instructions.cpp — these tests
/// validate correctness and serve as baseline for future branchless conversion.
/// </summary>
class PceCpuCmpComparisonTest : public ::testing::Test {
protected:
	// Current branching CMP (matches PceCpu.Instructions.cpp)
	static uint8_t CMP_Branching(uint8_t ps, uint8_t reg, uint8_t value) {
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		auto result = reg - value;
		if (reg >= value) ps |= PceCpuFlags::Carry;
		if (reg == value) ps |= PceCpuFlags::Zero;
		if ((result & 0x80) == 0x80) ps |= PceCpuFlags::Negative;
		return ps;
	}

	// Future branchless CMP
	static uint8_t CMP_Branchless(uint8_t ps, uint8_t reg, uint8_t value) {
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		auto result = reg - value;
		ps |= (reg >= value) ? PceCpuFlags::Carry : 0;
		ps |= ((uint8_t)result == 0) ? PceCpuFlags::Zero : 0;
		ps |= (result & 0x80);
		return ps;
	}
};

TEST_F(PceCpuCmpComparisonTest, Exhaustive_All256x256) {
	for (int r = 0; r < 256; r++) {
		for (int v = 0; v < 256; v++) {
			uint8_t reg = static_cast<uint8_t>(r);
			uint8_t val = static_cast<uint8_t>(v);
			uint8_t oldPS = CMP_Branching(0x00, reg, val);
			uint8_t newPS = CMP_Branchless(0x00, reg, val);
			ASSERT_EQ(oldPS, newPS)
				<< "CMP mismatch: reg=0x" << std::hex << (int)reg
				<< " value=0x" << (int)val;
		}
	}
}

TEST_F(PceCpuCmpComparisonTest, Equal_SetsZeroAndCarry) {
	uint8_t ps = CMP_Branchless(0x00, 0x42, 0x42);
	EXPECT_TRUE(ps & PceCpuFlags::Zero);
	EXPECT_TRUE(ps & PceCpuFlags::Carry);
	EXPECT_FALSE(ps & PceCpuFlags::Negative);
}

TEST_F(PceCpuCmpComparisonTest, RegGreater_SetsCarry) {
	uint8_t ps = CMP_Branchless(0x00, 0x50, 0x30);
	EXPECT_FALSE(ps & PceCpuFlags::Zero);
	EXPECT_TRUE(ps & PceCpuFlags::Carry);
}

TEST_F(PceCpuCmpComparisonTest, RegSmaller_SetsNegative) {
	uint8_t ps = CMP_Branchless(0x00, 0x30, 0x50);
	EXPECT_FALSE(ps & PceCpuFlags::Zero);
	EXPECT_FALSE(ps & PceCpuFlags::Carry);
	EXPECT_TRUE(ps & PceCpuFlags::Negative);  // 0x30-0x50 = 0xE0 (bit 7 set)
}

//=============================================================================
// Before/After Comparison: PCE ASL/LSR/ROL/ROR Shift Operations
//=============================================================================

/// <summary>
/// PCE shift comparison tests. These instructions clear C+Z+N before calling
/// SetZeroNegativeFlags, so the branchless path is exercised through the
/// Z/N flag setting. Tests validate the full instruction behavior.
/// </summary>
class PceCpuShiftComparisonTest : public ::testing::Test {
protected:
	struct ShiftResult {
		uint8_t result;
		uint8_t ps;
	};

	static ShiftResult ASL(uint8_t ps, uint8_t value) {
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		if (value & 0x80) ps |= PceCpuFlags::Carry;
		uint8_t result = value << 1;
		// Apply branchless SetZeroNegativeFlags
		ps |= (result == 0) ? PceCpuFlags::Zero : 0;
		ps |= (result & PceCpuFlags::Negative);
		return { result, ps };
	}

	static ShiftResult LSR(uint8_t ps, uint8_t value) {
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		if (value & 0x01) ps |= PceCpuFlags::Carry;
		uint8_t result = value >> 1;
		ps |= (result == 0) ? PceCpuFlags::Zero : 0;
		ps |= (result & PceCpuFlags::Negative);
		return { result, ps };
	}

	static ShiftResult ROL(uint8_t ps, uint8_t value) {
		bool carryIn = (ps & PceCpuFlags::Carry) != 0;
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		if (value & 0x80) ps |= PceCpuFlags::Carry;
		uint8_t result = (value << 1) | (carryIn ? 0x01 : 0x00);
		ps |= (result == 0) ? PceCpuFlags::Zero : 0;
		ps |= (result & PceCpuFlags::Negative);
		return { result, ps };
	}

	static ShiftResult ROR(uint8_t ps, uint8_t value) {
		bool carryIn = (ps & PceCpuFlags::Carry) != 0;
		ps &= ~(PceCpuFlags::Carry | PceCpuFlags::Negative | PceCpuFlags::Zero);
		if (value & 0x01) ps |= PceCpuFlags::Carry;
		uint8_t result = (value >> 1) | (carryIn ? 0x80 : 0x00);
		ps |= (result == 0) ? PceCpuFlags::Zero : 0;
		ps |= (result & PceCpuFlags::Negative);
		return { result, ps };
	}
};

TEST_F(PceCpuShiftComparisonTest, ASL_Exhaustive256) {
	for (int v = 0; v <= 255; v++) {
		uint8_t value = static_cast<uint8_t>(v);
		auto r = ASL(0x00, value);
		EXPECT_EQ(r.result, static_cast<uint8_t>(value << 1))
			<< "ASL result mismatch for value=0x" << std::hex << v;
		if (value & 0x80) EXPECT_TRUE(r.ps & PceCpuFlags::Carry);
		else EXPECT_FALSE(r.ps & PceCpuFlags::Carry);
	}
}

TEST_F(PceCpuShiftComparisonTest, LSR_Exhaustive256) {
	for (int v = 0; v <= 255; v++) {
		uint8_t value = static_cast<uint8_t>(v);
		auto r = LSR(0x00, value);
		EXPECT_EQ(r.result, static_cast<uint8_t>(value >> 1))
			<< "LSR result mismatch for value=0x" << std::hex << v;
		if (value & 0x01) EXPECT_TRUE(r.ps & PceCpuFlags::Carry);
		else EXPECT_FALSE(r.ps & PceCpuFlags::Carry);
		EXPECT_FALSE(r.ps & PceCpuFlags::Negative);  // LSR always clears bit 7
	}
}

TEST_F(PceCpuShiftComparisonTest, ROL_Exhaustive256x2) {
	for (int carryIn = 0; carryIn <= 1; carryIn++) {
		uint8_t initialPS = carryIn ? PceCpuFlags::Carry : 0;
		for (int v = 0; v <= 255; v++) {
			uint8_t value = static_cast<uint8_t>(v);
			auto r = ROL(initialPS, value);
			uint8_t expected = (value << 1) | (carryIn ? 1 : 0);
			EXPECT_EQ(r.result, expected)
				<< "ROL mismatch for value=0x" << std::hex << v
				<< " carry=" << carryIn;
		}
	}
}

TEST_F(PceCpuShiftComparisonTest, ROR_Exhaustive256x2) {
	for (int carryIn = 0; carryIn <= 1; carryIn++) {
		uint8_t initialPS = carryIn ? PceCpuFlags::Carry : 0;
		for (int v = 0; v <= 255; v++) {
			uint8_t value = static_cast<uint8_t>(v);
			auto r = ROR(initialPS, value);
			uint8_t expected = (value >> 1) | (carryIn ? 0x80 : 0x00);
			EXPECT_EQ(r.result, expected)
				<< "ROR mismatch for value=0x" << std::hex << v
				<< " carry=" << carryIn;
		}
	}
}
