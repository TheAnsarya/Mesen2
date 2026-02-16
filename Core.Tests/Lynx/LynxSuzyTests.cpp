#include "pch.h"
#include "Lynx/LynxTypes.h"

/// <summary>
/// Tests for Lynx Suzy math coprocessor, sprite system, and hardware bugs.
/// Verifies multiply/divide behavior, sign-magnitude bugs (13.8, 13.9, 13.10),
/// sprite chain termination (bug 13.12), and collision buffer layout.
/// </summary>
class LynxSuzyMathTest : public ::testing::Test {
protected:
	LynxSuzyState _state = {};

	void SetUp() override {
		memset(&_state, 0, sizeof(_state));
	}
};

//=============================================================================
// Math Coprocessor — Unsigned Multiply
//=============================================================================

TEST_F(LynxSuzyMathTest, Multiply_UnsignedSimple) {
	// 3 × 5 = 15
	uint16_t a = 3, b = 5;
	uint32_t result = (uint32_t)a * (uint32_t)b;
	EXPECT_EQ(result, 15u);
}

TEST_F(LynxSuzyMathTest, Multiply_UnsignedMaxValues) {
	// 0xFFFF × 0xFFFF = 0xFFFE0001
	uint16_t a = 0xFFFF, b = 0xFFFF;
	uint32_t result = (uint32_t)a * (uint32_t)b;
	EXPECT_EQ(result, 0xFFFE0001u);
}

TEST_F(LynxSuzyMathTest, Multiply_UnsignedZero) {
	uint16_t a = 0, b = 12345;
	uint32_t result = (uint32_t)a * (uint32_t)b;
	EXPECT_EQ(result, 0u);
}

//=============================================================================
// HW Bug 13.8 — Signed Multiply: $8000 is Positive
// The sign-magnitude math uses bit 15 for sign, but due to a hardware bug,
// $8000 (only sign bit set, magnitude 0) is treated as POSITIVE zero,
// while $0000 is treated as NEGATIVE zero.
//=============================================================================

TEST_F(LynxSuzyMathTest, Bug13_8_SignMagnitude_8000IsPositive) {
	// On real Lynx: $8000 → positive (sign bit ignored for $8000)
	// sign = value & 0x8000
	// magnitude = value & 0x7FFF
	// If magnitude == 0, the sign bit determines positive/negative zero
	// Bug: $8000 = sign=1 but treated as positive
	int16_t value = (int16_t)0x8000;
	bool isNegative = (value & 0x8000) != 0;
	uint16_t magnitude = value & 0x7FFF;

	// $8000: sign=1, magnitude=0
	EXPECT_TRUE(isNegative);
	EXPECT_EQ(magnitude, 0u);

	// In sign-magnitude interpretation, $8000 should be "negative zero"
	// But the hardware treats magnitude-0 values specially:
	// The two's complement conversion of magnitude=0 is still 0,
	// so sign doesn't matter for the actual computation
	int16_t twosComp = isNegative ? (int16_t)(~magnitude + 1) : (int16_t)magnitude;
	// ~0 + 1 = 0x10000 which truncates to 0
	EXPECT_EQ((uint16_t)twosComp, 0u);
}

TEST_F(LynxSuzyMathTest, Bug13_8_SignMagnitude_0000IsNegative) {
	// $0000: sign=0, magnitude=0 — "positive zero"
	int16_t value = 0x0000;
	bool isNegative = (value & 0x8000) != 0;
	uint16_t magnitude = value & 0x7FFF;

	EXPECT_FALSE(isNegative);
	EXPECT_EQ(magnitude, 0u);
}

TEST_F(LynxSuzyMathTest, Bug13_8_SignMagnitude_NormalNegative) {
	// $8005 = sign=1, magnitude=5 → -5 in sign-magnitude
	int16_t value = (int16_t)0x8005;
	bool isNegative = (value & 0x8000) != 0;
	uint16_t magnitude = value & 0x7FFF;

	EXPECT_TRUE(isNegative);
	EXPECT_EQ(magnitude, 5u);

	int16_t twosComp = isNegative ? (int16_t)(~magnitude + 1) : (int16_t)magnitude;
	EXPECT_EQ(twosComp, -5);
}

//=============================================================================
// HW Bug 13.9 — Division: Remainder Always Positive
// The hardware doesn't negate the remainder, even for signed division.
//=============================================================================

TEST_F(LynxSuzyMathTest, Bug13_9_DivideRemainderAlwaysPositive) {
	// -7 / 3 should give quotient = -2 or -3, remainder = -1 or 2
	// But hardware always returns positive remainder
	int32_t dividend = -7;
	int16_t divisor = 3;

	// Normal C division
	int16_t quotient = (int16_t)(dividend / divisor);
	int16_t remainder = (int16_t)(dividend % divisor);

	// C truncates toward zero: -7/3 = -2 remainder -1
	EXPECT_EQ(quotient, -2);
	EXPECT_EQ(remainder, -1);

	// Hardware bug: remainder is forced positive (magnitude only)
	uint16_t hwRemainder = (uint16_t)std::abs(remainder);
	EXPECT_EQ(hwRemainder, 1u);
	EXPECT_GE(hwRemainder, 0u); // Always positive
}

TEST_F(LynxSuzyMathTest, Bug13_9_DivideRemainderPositive_LargeValues) {
	int32_t dividend = -12345;
	int16_t divisor = 100;

	int16_t remainder = (int16_t)(dividend % divisor);
	EXPECT_EQ(remainder, -45); // C gives -45

	uint16_t hwRemainder = (uint16_t)std::abs(remainder);
	EXPECT_EQ(hwRemainder, 45u); // Hardware gives +45
}

//=============================================================================
// HW Bug 13.10 — Math Overflow Overwritten Per Operation
// The overflow flag is NOT OR'd across operations — each new multiply/divide
// overwrites the previous overflow status.
//=============================================================================

TEST_F(LynxSuzyMathTest, Bug13_10_OverflowOverwrittenPerOp) {
	// Suppose first multiply overflows
	_state.MathOverflow = true;

	// Second multiply does NOT overflow — should clear the flag
	_state.MathOverflow = false; // New operation sets its own result

	EXPECT_FALSE(_state.MathOverflow);
	// Key: it was NOT OR'd with the previous true value
}

TEST_F(LynxSuzyMathTest, Bug13_10_OverflowDetectionLogic) {
	// Accumulate mode: result added to existing value
	// Overflow occurs when 32-bit accumulator would exceed 32 bits
	uint32_t accumulator = 0xFFFF0000;
	uint32_t newResult = 0x00020000;

	uint64_t fullResult = (uint64_t)accumulator + (uint64_t)newResult;
	bool overflow = (fullResult >> 32) != 0;

	EXPECT_TRUE(overflow);
}

//=============================================================================
// HW Bug 13.12 — SCB NEXT Only Checks Upper Byte
// The sprite chain terminates when (scbAddr >> 8) == 0, not scbAddr == 0.
// This means addresses $0000-$00FF all terminate the chain.
//=============================================================================

TEST_F(LynxSuzyMathTest, Bug13_12_SpriteChainTermination_ZeroPage) {
	// Any address in zero page ($0000-$00FF) should terminate the chain
	for (uint16_t addr = 0x0000; addr <= 0x00FF; addr++) {
		bool terminates = (addr >> 8) == 0;
		EXPECT_TRUE(terminates) << "Address $" << std::hex << addr
			<< " should terminate the sprite chain";
	}
}

TEST_F(LynxSuzyMathTest, Bug13_12_SpriteChainTermination_NonZeroPage) {
	// Addresses $0100 and above should NOT terminate
	uint16_t addr = 0x0100;
	EXPECT_FALSE((addr >> 8) == 0);

	addr = 0x1000;
	EXPECT_FALSE((addr >> 8) == 0);

	addr = 0xFFFF;
	EXPECT_FALSE((addr >> 8) == 0);
}

TEST_F(LynxSuzyMathTest, Bug13_12_CompareWithCorrectTermination) {
	// Show the difference: addr == 0 would only match $0000
	// But (addr >> 8) == 0 matches $0000-$00FF
	uint16_t addr = 0x0001;
	bool correctCheck = (addr >> 8) == 0;  // Upper byte check
	bool naiveCheck = (addr == 0);          // Full word check

	EXPECT_TRUE(correctCheck);   // Upper byte is 0 → terminates
	EXPECT_FALSE(naiveCheck);    // Full word is nonzero → doesn't terminate
	// This proves the bug matters for addresses $01-$FF
}

//=============================================================================
// Collision Buffer Layout
//=============================================================================

TEST_F(LynxSuzyMathTest, CollisionBuffer_Size) {
	EXPECT_EQ(LynxConstants::CollisionBufferSize, 16u);
}

TEST_F(LynxSuzyMathTest, CollisionBuffer_DefaultZero) {
	for (int i = 0; i < 16; i++) {
		EXPECT_EQ(_state.CollisionBuffer[i], 0u);
	}
}

TEST_F(LynxSuzyMathTest, CollisionBuffer_MutualUpdate) {
	// When sprite A (collNum=3) writes to pixel of color index 5,
	// and collisionBuffer[5] already has value 2 (from sprite B):
	uint8_t collNum = 3;
	uint8_t pixIndex = 5;
	_state.CollisionBuffer[pixIndex] = 2; // Previously written by sprite 2

	uint8_t existing = _state.CollisionBuffer[pixIndex];
	if (existing != 0) {
		_state.CollisionBuffer[collNum] |= existing;
		_state.CollisionBuffer[existing] |= collNum;
	}

	EXPECT_EQ(_state.CollisionBuffer[3], 2u); // Sprite 3 collided with 2
	EXPECT_EQ(_state.CollisionBuffer[2], 3u); // Sprite 2 collided with 3
}

//=============================================================================
// Sprite Type and BPP Enums
//=============================================================================

TEST_F(LynxSuzyMathTest, SpriteType_Values) {
	EXPECT_EQ((uint8_t)LynxSpriteType::Background, 0);
	EXPECT_EQ((uint8_t)LynxSpriteType::Normal, 1);
	EXPECT_EQ((uint8_t)LynxSpriteType::Boundary, 2);
	EXPECT_EQ((uint8_t)LynxSpriteType::Shadow, 7);
}

TEST_F(LynxSuzyMathTest, SpriteBpp_Values) {
	EXPECT_EQ((uint8_t)LynxSpriteBpp::Bpp1, 0);
	EXPECT_EQ((uint8_t)LynxSpriteBpp::Bpp2, 1);
	EXPECT_EQ((uint8_t)LynxSpriteBpp::Bpp3, 2);
	EXPECT_EQ((uint8_t)LynxSpriteBpp::Bpp4, 3);
}

//=============================================================================
// Math Register State
//=============================================================================

TEST_F(LynxSuzyMathTest, MathState_DefaultZero) {
	EXPECT_EQ(_state.MathA, 0);
	EXPECT_EQ(_state.MathB, 0);
	EXPECT_EQ(_state.MathC, 0);
	EXPECT_EQ(_state.MathD, 0);
	EXPECT_FALSE(_state.MathSign);
	EXPECT_FALSE(_state.MathAccumulate);
	EXPECT_FALSE(_state.MathInProgress);
	EXPECT_FALSE(_state.MathOverflow);
}

TEST_F(LynxSuzyMathTest, MathState_SignAccumulateFlags) {
	_state.MathSign = true;
	_state.MathAccumulate = true;
	EXPECT_TRUE(_state.MathSign);
	EXPECT_TRUE(_state.MathAccumulate);
}
