#pragma once
#include "pch.h"
#include <span>
#include <vector>
#include <cstdint>
#include <optional>

/// @file LynxDecrypt.h
/// @brief Atari Lynx cartridge bootloader RSA decryption utility.
///
/// The Lynx uses RSA encryption to protect the initial bootloader code
/// on cartridges. The boot ROM contains a decryption routine that uses
/// Montgomery multiplication to perform modular exponentiation.
///
/// **Algorithm Details:**
///   - RSA modular exponentiation: `PLAINTEXT = ENCRYPTED³ mod PUBLIC_KEY`
///   - Public exponent: 3 (hardcoded for performance)
///   - Block size: 51 bytes (408 bits)
///   - Montgomery multiplication for efficient computation without division
///
/// **Cartridge Data Format:**
///   - Byte 0: Block count byte (N = 256 - value)
///   - Bytes 1+: N × 51 bytes of encrypted data
///   - Each block decrypts to 50 bytes of output
///
/// **Note:** The private key is permanently lost, so re-encryption is
/// NOT possible. Homebrew developers use pre-encrypted bootloaders
/// created by Harry Dodgson.
///
/// **References:**
///   - libretro-handy `lynxdec.cpp` (zlib license)
///   - AtariAge "Lynx Encryption?" thread
///   - ~docs/plans/lynx-bootloader-encryption-research.md
///
/// @see https://github.com/libretro/libretro-handy/blob/master/lynx/lynxdec.cpp

namespace LynxCrypto {

/// <summary>
/// RSA public modulus embedded in the Lynx boot ROM.
/// This 51-byte (408-bit) value is the same for all Lynx units.
/// </summary>
/// <remarks>
/// The private key was encrypted with a secondary key that was destroyed.
/// Only decryption using this public modulus is possible.
/// </remarks>
inline constexpr uint8_t PublicModulus[51] = {
	0x35, 0xB5, 0xA3, 0x94, 0x28, 0x06, 0xD8, 0xA2,
	0x26, 0x95, 0xD7, 0x71, 0xB2, 0x3C, 0xFD, 0x56,
	0x1C, 0x4A, 0x19, 0xB6, 0xA3, 0xB0, 0x26, 0x00,
	0x36, 0x5A, 0x30, 0x6E, 0x3C, 0x4D, 0x63, 0x38,
	0x1B, 0xD4, 0x1C, 0x13, 0x64, 0x89, 0x36, 0x4C,
	0xF2, 0xBA, 0x2A, 0x58, 0xF4, 0xFE, 0xE1, 0xFD,
	0xAC, 0x7E, 0x79
};

/// <summary>Block size for RSA encryption (51 bytes = 408 bits).</summary>
inline constexpr size_t BlockSize = 51;

/// <summary>Output bytes per decrypted block (50 of 51 bytes).</summary>
inline constexpr size_t OutputBytesPerBlock = 50;

/// <summary>Minimum valid encrypted data size (1 block count byte + 1 block).</summary>
inline constexpr size_t MinEncryptedSize = 1 + BlockSize;

/// <summary>
/// Decryption result containing the plaintext data and validation status.
/// </summary>
struct DecryptResult {
	/// <summary>Decrypted plaintext data.</summary>
	std::vector<uint8_t> Data;

	/// <summary>True if decryption succeeded and checksum validated.</summary>
	bool Valid = false;

	/// <summary>Number of encrypted blocks processed.</summary>
	size_t BlockCount = 0;

	/// <summary>Final accumulator value (should be 0 for valid data).</summary>
	uint8_t Checksum = 0;
};

/// <summary>
/// Decrypt RSA-encrypted Lynx cartridge bootloader data.
/// </summary>
/// <param name="encrypted">
/// Encrypted data from cartridge offset 0 (after LNX header if present).
/// Must contain at least 52 bytes (1 block count + 51-byte block).
/// </param>
/// <returns>
/// DecryptResult containing decrypted data and validation status.
/// Returns empty result with Valid=false if input is malformed.
/// </returns>
/// <remarks>
/// **Algorithm:**
/// 1. Read block count from byte 0 (N = 256 - value)
/// 2. For each 51-byte block, compute B³ mod PublicModulus
/// 3. Accumulate output bytes with running checksum
/// 4. Valid data has final checksum of 0
///
/// **Example:**
/// ```cpp
/// auto result = LynxCrypto::Decrypt(cartData);
/// if (result.Valid) {
///     // Use result.Data (decrypted loader)
/// }
/// ```
/// </remarks>
[[nodiscard]] DecryptResult Decrypt(std::span<const uint8_t> encrypted);

/// <summary>
/// Validate encrypted cartridge data structure without full decryption.
/// </summary>
/// <param name="encrypted">Encrypted data from cartridge.</param>
/// <returns>True if data appears to be validly formatted encrypted data.</returns>
/// <remarks>
/// Checks:
/// - Minimum size (52 bytes)
/// - Block count is reasonable (1-15 blocks typically)
/// - Sufficient data for declared block count
/// This does NOT verify the data decrypts correctly.
/// </remarks>
[[nodiscard]] bool Validate(std::span<const uint8_t> encrypted);

/// <summary>
/// Calculate the decrypted output size from encrypted data.
/// </summary>
/// <param name="encrypted">Encrypted data from cartridge.</param>
/// <returns>
/// Expected decrypted size in bytes, or 0 if data is malformed.
/// </returns>
/// <remarks>
/// Formula: `(256 - encrypted[0]) * 50` bytes
/// Typical values:
/// - 8 blocks (0xF8) → 400 bytes
/// - 5 blocks (0xFB) → 250 bytes
/// </remarks>
[[nodiscard]] size_t GetDecryptedSize(std::span<const uint8_t> encrypted);

/// <summary>
/// Get the number of encrypted blocks in the data.
/// </summary>
/// <param name="encrypted">Encrypted data from cartridge.</param>
/// <returns>Block count (N = 256 - encrypted[0]), or 0 if malformed.</returns>
[[nodiscard]] size_t GetBlockCount(std::span<const uint8_t> encrypted);

/// <summary>
/// Get the required encrypted data size for a given block count.
/// </summary>
/// <param name="blockCount">Number of 51-byte encrypted blocks.</param>
/// <returns>Total bytes required (1 + blockCount * 51).</returns>
[[nodiscard]] constexpr size_t GetRequiredSize(size_t blockCount) {
	return 1 + blockCount * BlockSize;
}

// ============================================================================
// Low-Level API (for advanced use)
// ============================================================================

/// <summary>
/// Decrypt a single 51-byte encrypted block.
/// </summary>
/// <param name="output">Output buffer (50 bytes).</param>
/// <param name="block">Input encrypted block (51 bytes).</param>
/// <param name="accumulator">
/// Running accumulator value (modified in-place).
/// Initialize to 0 for first block.
/// </param>
/// <remarks>
/// Computes `B³ mod PublicModulus` using Montgomery multiplication,
/// then applies accumulator-based obfuscation.
/// </remarks>
void DecryptBlock(
	std::span<uint8_t, OutputBytesPerBlock> output,
	std::span<const uint8_t, BlockSize> block,
	uint8_t& accumulator
);

/// <summary>
/// Perform Montgomery multiplication: L = M × N mod modulus.
/// </summary>
/// <param name="result">Output buffer (51 bytes).</param>
/// <param name="multiplicand">First operand (51 bytes).</param>
/// <param name="multiplier">Second operand (51 bytes).</param>
/// <param name="modulus">Modulus (typically PublicModulus, 51 bytes).</param>
/// <remarks>
/// Uses the Montgomery multiplication algorithm to compute
/// modular multiplication without hardware division. This is the
/// same algorithm used by the Lynx boot ROM on the 65C02.
///
/// **Performance:** O(n²) where n = 51 bytes = 408 bits.
/// Approximately 408² = 166,464 bit operations per multiplication.
/// </remarks>
void MontgomeryMultiply(
	std::span<uint8_t, BlockSize> result,
	std::span<const uint8_t, BlockSize> multiplicand,
	std::span<const uint8_t, BlockSize> multiplier,
	std::span<const uint8_t, BlockSize> modulus
);

} // namespace LynxCrypto
