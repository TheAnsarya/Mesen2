#include "pch.h"
#include "Utilities/HexUtilities.h"

// Test fixture for HexUtilities
class HexUtilitiesTest : public ::testing::Test {};

// ===== ToHex(uint8_t) Tests =====

TEST_F(HexUtilitiesTest, ToHex_Uint8_Zero) {
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0), "00");
}

TEST_F(HexUtilitiesTest, ToHex_Uint8_Max) {
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0xFF), "FF");
}

TEST_F(HexUtilitiesTest, ToHex_Uint8_MidValues) {
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0x0A), "0A");
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0x42), "42");
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0x80), "80");
	EXPECT_EQ(HexUtilities::ToHex((uint8_t)0xAB), "AB");
}

// ===== ToHex(uint16_t) Tests =====

TEST_F(HexUtilitiesTest, ToHex_Uint16_Zero) {
	EXPECT_EQ(HexUtilities::ToHex((uint16_t)0), "0000");
}

TEST_F(HexUtilitiesTest, ToHex_Uint16_Max) {
	EXPECT_EQ(HexUtilities::ToHex((uint16_t)0xFFFF), "FFFF");
}

TEST_F(HexUtilitiesTest, ToHex_Uint16_MidValues) {
	EXPECT_EQ(HexUtilities::ToHex((uint16_t)0x1234), "1234");
	EXPECT_EQ(HexUtilities::ToHex((uint16_t)0xABCD), "ABCD");
	EXPECT_EQ(HexUtilities::ToHex((uint16_t)0x00FF), "00FF");
}

// ===== ToHex(uint32_t) Tests =====

TEST_F(HexUtilitiesTest, ToHex_Uint32_SmallValue_ReturnsShort) {
	// Without fullSize, small values use minimal digits
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x42, false), "42");
}

TEST_F(HexUtilitiesTest, ToHex_Uint32_16BitValue_Returns4Digits) {
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x1234, false), "1234");
}

TEST_F(HexUtilitiesTest, ToHex_Uint32_24BitValue_Returns6Digits) {
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x123456, false), "123456");
}

TEST_F(HexUtilitiesTest, ToHex_Uint32_FullValue_Returns8Digits) {
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x12345678, false), "12345678");
}

TEST_F(HexUtilitiesTest, ToHex_Uint32_FullSize_AlwaysReturns8Digits) {
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x42, true), "00000042");
	EXPECT_EQ(HexUtilities::ToHex((uint32_t)0x1234, true), "00001234");
}

// ===== ToHex32 Tests =====

TEST_F(HexUtilitiesTest, ToHex32_Zero) {
	EXPECT_EQ(HexUtilities::ToHex32(0), "00000000");
}

TEST_F(HexUtilitiesTest, ToHex32_Max) {
	EXPECT_EQ(HexUtilities::ToHex32(0xFFFFFFFF), "FFFFFFFF");
}

TEST_F(HexUtilitiesTest, ToHex32_KnownValue) {
	EXPECT_EQ(HexUtilities::ToHex32(0xDEADBEEF), "DEADBEEF");
}

// ===== ToHex24 Tests =====

TEST_F(HexUtilitiesTest, ToHex24_Zero) {
	EXPECT_EQ(HexUtilities::ToHex24(0), "000000");
}

TEST_F(HexUtilitiesTest, ToHex24_KnownValue) {
	EXPECT_EQ(HexUtilities::ToHex24(0x7E2000), "7E2000");
}

// ===== ToHex20 Tests =====

TEST_F(HexUtilitiesTest, ToHex20_Zero) {
	EXPECT_EQ(HexUtilities::ToHex20(0), "00000");
}

TEST_F(HexUtilitiesTest, ToHex20_KnownValue) {
	EXPECT_EQ(HexUtilities::ToHex20(0xFFFFF), "FFFFF");
}

// ===== ToHex(uint64_t) Tests =====

TEST_F(HexUtilitiesTest, ToHex_Uint64_Zero) {
	EXPECT_EQ(HexUtilities::ToHex((uint64_t)0), "0000000000000000");
}

TEST_F(HexUtilitiesTest, ToHex_Uint64_Max) {
	EXPECT_EQ(HexUtilities::ToHex((uint64_t)0xFFFFFFFFFFFFFFFF), "FFFFFFFFFFFFFFFF");
}

TEST_F(HexUtilitiesTest, ToHex_Uint64_KnownValue) {
	EXPECT_EQ(HexUtilities::ToHex((uint64_t)0x0123456789ABCDEF), "0123456789ABCDEF");
}

// ===== ToHex(vector<uint8_t>&) Tests =====

TEST_F(HexUtilitiesTest, ToHex_Vector_NoDelimiter) {
	std::vector<uint8_t> data = {0xDE, 0xAD, 0xBE, 0xEF};
	EXPECT_EQ(HexUtilities::ToHex(data), "DEADBEEF");
}

TEST_F(HexUtilitiesTest, ToHex_Vector_WithDelimiter) {
	std::vector<uint8_t> data = {0xCA, 0xFE};
	EXPECT_EQ(HexUtilities::ToHex(data, ' '), "CA FE ");
}

TEST_F(HexUtilitiesTest, ToHex_Vector_Empty) {
	std::vector<uint8_t> data = {};
	EXPECT_EQ(HexUtilities::ToHex(data), "");
}

TEST_F(HexUtilitiesTest, ToHex_Vector_SingleByte) {
	std::vector<uint8_t> data = {0x42};
	EXPECT_EQ(HexUtilities::ToHex(data), "42");
}

// ===== FromHex Tests =====

TEST_F(HexUtilitiesTest, FromHex_Uppercase) {
	EXPECT_EQ(HexUtilities::FromHex("FF"), 0xFF);
	EXPECT_EQ(HexUtilities::FromHex("DEADBEEF"), 0xDEADBEEF);
}

TEST_F(HexUtilitiesTest, FromHex_Lowercase) {
	EXPECT_EQ(HexUtilities::FromHex("ff"), 0xFF);
	EXPECT_EQ(HexUtilities::FromHex("abcd"), 0xABCD);
}

TEST_F(HexUtilitiesTest, FromHex_MixedCase) {
	EXPECT_EQ(HexUtilities::FromHex("AbCd"), 0xABCD);
}

TEST_F(HexUtilitiesTest, FromHex_SingleDigit) {
	EXPECT_EQ(HexUtilities::FromHex("0"), 0);
	EXPECT_EQ(HexUtilities::FromHex("F"), 0xF);
}

TEST_F(HexUtilitiesTest, FromHex_Zero) {
	EXPECT_EQ(HexUtilities::FromHex("00"), 0);
	EXPECT_EQ(HexUtilities::FromHex("0000"), 0);
}

TEST_F(HexUtilitiesTest, FromHex_EmptyString) {
	EXPECT_EQ(HexUtilities::FromHex(""), 0);
}

// ===== ToHexChar Tests =====

TEST_F(HexUtilitiesTest, ToHexChar_ReturnsValidCString) {
	const char* result = HexUtilities::ToHexChar(0xAB);
	EXPECT_STREQ(result, "AB");
}

TEST_F(HexUtilitiesTest, ToHexChar_Zero) {
	EXPECT_STREQ(HexUtilities::ToHexChar(0), "00");
}

TEST_F(HexUtilitiesTest, ToHexChar_Max) {
	EXPECT_STREQ(HexUtilities::ToHexChar(0xFF), "FF");
}

// ===== Roundtrip Tests =====

TEST_F(HexUtilitiesTest, Roundtrip_Uint8) {
	for (int i = 0; i <= 255; i++) {
		std::string hex = HexUtilities::ToHex((uint8_t)i);
		int parsed = HexUtilities::FromHex(hex);
		EXPECT_EQ(parsed, i) << "Failed roundtrip for value " << i;
	}
}

TEST_F(HexUtilitiesTest, Roundtrip_Uint16) {
	// Test boundary values and a few mid-range
	uint16_t values[] = {0, 1, 0xFF, 0x100, 0x1234, 0x7FFF, 0x8000, 0xFFFF};
	for (uint16_t val : values) {
		std::string hex = HexUtilities::ToHex(val);
		int parsed = HexUtilities::FromHex(hex);
		EXPECT_EQ(parsed, val) << "Failed roundtrip for " << val;
	}
}
