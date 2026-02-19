#include "pch.h"
#include "Lynx/LynxSuzy.h"
#include "Lynx/LynxConsole.h"
#include "Lynx/LynxCpu.h"
#include "Lynx/LynxMemoryManager.h"
#include "Lynx/LynxCart.h"
#include "Shared/Emulator.h"
#include "Utilities/Serializer.h"

void LynxSuzy::Init(Emulator* emu, LynxConsole* console, LynxMemoryManager* memoryManager, LynxCart* cart) {
	_emu = emu;
	_console = console;
	_memoryManager = memoryManager;
	_cart = cart;

	memset(&_state, 0, sizeof(_state));
	memset(_lineBuffer, 0, sizeof(_lineBuffer));

	_state.Joystick = 0xff; // All buttons released (active-low)
	_state.Switches = 0xff;

	// Hardware defaults (matching Handy's Reset)
	_state.HSizeOff = 0x007f;
	_state.VSizeOff = 0x007f;
}

__forceinline uint8_t LynxSuzy::ReadRam(uint16_t addr) {
	if (_spriteProcessingActive) _spriteBusCycles++;
	return _console->GetWorkRam()[addr & 0xFFFF];
}

__forceinline uint16_t LynxSuzy::ReadRam16(uint16_t addr) {
	uint8_t lo = ReadRam(addr);
	uint8_t hi = ReadRam(addr + 1);
	return (uint16_t)(hi << 8) | lo;
}

void LynxSuzy::DoMultiply() {
	// 16x16 → 32-bit multiply
	// MATHC:MATHD × MATHE:MATHF → MATHG:MATHH:MATHJ:MATHK
	_state.MathInProgress = true;

	// HW Bug 13.10: The math overflow flag is OVERWRITTEN by each new operation,
	// not OR'd. A previous overflow is lost if the CPU doesn't read SPRSYS before
	// the next math operation completes. We clear it at the start of each operation.
	_state.MathOverflow = false;
	_state.LastCarry = false;

	if (_state.MathSign) {
		// Signed multiply
		// HW Bug 13.8: The hardware sign-detection logic has two errors:
		//   1. $8000 is treated as POSITIVE (the sign bit is checked, but the
		//      negate-and-complement produces $8000 again, so it stays positive)
		//   2. $0000 with the sign flag is treated as NEGATIVE (negate of 0 =
		//      $10000 which truncates to $0000, but sign is still set)
		// We emulate this by converting to sign-magnitude the way the HW does:
		//   - Check bit 15 of each operand for sign
		//   - If set, negate (two's complement) the value
		//   - Multiply as unsigned
		//   - If signs differ, negate the result
		// The bug manifests because negating $8000 = $8000 (positive $8000)
		// and negating $0000 = $0000 but still flagged as negative.
		uint16_t a = (uint16_t)_state.MathC;
		uint16_t b = (uint16_t)_state.MathE;
		bool aNeg = (a & 0x8000) != 0;
		bool bNeg = (b & 0x8000) != 0;

		// Hardware performs two's complement if sign bit set
		// Bug: ~$8000 + 1 = $7FFF + 1 = $8000 (unchanged, treated as positive magnitude)
		// Bug: ~$0000 + 1 = $FFFF + 1 = $0000 (truncated, but sign still flagged)
		if (aNeg) a = (uint16_t)(~a + 1);
		if (bNeg) b = (uint16_t)(~b + 1);

		uint32_t result = (uint32_t)a * (uint32_t)b;

		// If signs differ, negate the result
		bool resultNeg = aNeg ^ bNeg;
		if (resultNeg) {
			result = ~result + 1;
		}

		if (_state.MathAccumulate) {
			uint32_t accum = ((uint32_t)_state.MathG << 16) | _state.MathH;
			uint64_t sum64 = (uint64_t)result + (uint64_t)accum;
			if (sum64 > 0xFFFFFFFF) {
				_state.MathOverflow = true;
				_state.LastCarry = true;
			}
			result = (uint32_t)sum64;
		}

		_state.MathG = (uint16_t)((result >> 16) & 0xffff);
		_state.MathH = (uint16_t)(result & 0xffff);
	} else {
		// Unsigned multiply
		uint32_t a = (uint16_t)_state.MathC;
		uint32_t b = (uint16_t)_state.MathE;
		uint32_t result = a * b;

		if (_state.MathAccumulate) {
			uint32_t accum = ((uint32_t)_state.MathG << 16) | _state.MathH;
			uint64_t sum64 = (uint64_t)result + (uint64_t)accum;
			if (sum64 > 0xFFFFFFFF) {
				_state.MathOverflow = true;
				_state.LastCarry = true;
			}
			result = (uint32_t)sum64;
		}

		_state.MathG = (uint16_t)((result >> 16) & 0xffff);
		_state.MathH = (uint16_t)(result & 0xffff);
	}

	_state.MathInProgress = false;
}

void LynxSuzy::DoDivide() {
	// 32÷16 → 16 quotient, 16 remainder
	// MATHG:MATHH ÷ MATHE → MATHC (quotient), MATHG (remainder)
	_state.MathInProgress = true;

	if (_state.MathE == 0) {
		// Division by zero
		_state.MathC = 0;
		_state.MathD = 0;
		_state.MathG = 0;
		_state.MathH = 0;
	} else if (_state.MathSign) {
		// HW Bug 13.9: Signed division remainder errors.
		// The hardware performs sign-magnitude division similar to multiply:
		// 1. Extract signs, negate to positive magnitude
		// 2. Divide as unsigned
		// 3. Negate quotient if signs differ
		// 4. The remainder follows the dividend's sign, BUT has the same
		//    $8000/$0000 bugs as multiply (Bug 13.8).
		uint32_t dividend = ((uint32_t)_state.MathG << 16) | (uint16_t)_state.MathH;
		uint16_t divisor = _state.MathE;
		bool dividendNeg = (dividend & 0x80000000) != 0;
		bool divisorNeg = (divisor & 0x8000) != 0;

		// Two's complement like the hardware does (same $8000 bug as multiply)
		if (dividendNeg) dividend = ~dividend + 1;
		if (divisorNeg) divisor = (uint16_t)(~divisor + 1);

		uint32_t quotient = dividend / (uint32_t)divisor;
		uint32_t remainder = dividend % (uint32_t)divisor;

		// Negate quotient if signs differ
		if (dividendNeg ^ divisorNeg) {
			quotient = ~quotient + 1;
		}

		// HW Bug 13.9: Remainder should follow dividend sign but the hardware
		// doesn't always negate it correctly — remainder is always positive
		// magnitude from the unsigned division. We match the hardware behavior:
		// remainder is NOT negated, regardless of dividend sign.

		_state.MathC = (int16_t)(quotient & 0xffff);
		_state.MathD = (int16_t)((quotient >> 16) & 0xffff);
		_state.MathG = (uint16_t)(remainder & 0xffff);
		_state.MathH = 0;
	} else {
		uint32_t dividend = ((uint32_t)_state.MathG << 16) | (uint16_t)_state.MathH;
		uint16_t divisor = _state.MathE;
		uint32_t quotient = dividend / divisor;
		uint32_t remainder = dividend % divisor;

		_state.MathC = (int16_t)(quotient & 0xffff);
		_state.MathD = (int16_t)((quotient >> 16) & 0xffff);
		_state.MathG = (uint16_t)(remainder & 0xffff);
		_state.MathH = 0;
	}

	_state.MathInProgress = false;
}

void LynxSuzy::ProcessSpriteChain() {
	if (!_state.SpriteEnabled) {
		_state.SpriteBusy = false;
		return;
	}

	_state.SpriteBusy = true;
	_spriteBusCycles = 0;
	_spriteProcessingActive = true;
	uint16_t scbAddr = _state.SCBAddress;

	// Walk the sprite linked list
	int spriteCount = 0;
	// HW Bug 13.12: The hardware only checks the UPPER BYTE of the SCB NEXT
	// address for zero to terminate the sprite chain. If the upper byte is
	// zero but the lower byte is non-zero (e.g., $0080), the chain still
	// terminates. Conversely, $0100 would NOT terminate (upper byte = $01).
	while ((scbAddr >> 8) != 0 && spriteCount < 256) { // Safety limit
		ProcessSprite(scbAddr);
		// Read next SCB pointer from SCB offset 3-4 (after CTL0, CTL1, COLL)
		scbAddr = ReadRam16(scbAddr + 3);
		spriteCount++;
	}

	_spriteProcessingActive = false;
	_state.SpriteBusy = false;

	// Apply bus contention -- stall CPU for cycles consumed by sprite processing.
	// On real hardware, the CPU is halted while Suzy owns the bus.
	// Each byte-wide bus access costs ~1 CPU cycle (5 master clocks / 4).
	if (_spriteBusCycles > 0) {
		_console->GetCpu()->AddCycles(_spriteBusCycles);
	}
}

void LynxSuzy::ProcessSprite(uint16_t scbAddr) {
	// SCB Layout (matching Handy/hardware):
	// Offset 0:    SPRCTL0 — sprite type, BPP, H/V flip
	// Offset 1:    SPRCTL1 — skip, reload, sizing, literal, quadrant
	// Offset 2:    SPRCOLL — collision number and flags
	// Offset 3-4:  SCBNEXT — link to next SCB (read in ProcessSpriteChain)
	// Offset 5-6:  SPRDLINE — sprite data pointer (always loaded)
	// Offset 7-8:  HPOSSTRT — horizontal position (always loaded)
	// Offset 9-10: VPOSSTRT — vertical position (always loaded)
	// Offset 11+:  Variable-length optional fields based on ReloadDepth

	uint8_t sprCtl0 = ReadRam(scbAddr);       // Offset 0: SPRCTL0
	uint8_t sprCtl1 = ReadRam(scbAddr + 1);   // Offset 1: SPRCTL1
	uint8_t sprColl = ReadRam(scbAddr + 2);   // Offset 2: SPRCOLL

	// SPRCTL1 bit 2: skip this sprite in the chain
	if (sprCtl1 & 0x04) {
		return;
	}

	// === SPRCTL0 decoding ===
	// Bits 7:6 = BPP: 00=1bpp, 01=2bpp, 10=3bpp, 11=4bpp
	// Bit 5 = H-flip (mirror horizontally)
	// Bit 4 = V-flip (mirror vertically)
	// Bits 2:0 = Sprite type (0-7)
	int bpp = ((sprCtl0 >> 6) & 0x03) + 1;
	LynxSpriteType spriteType = static_cast<LynxSpriteType>(sprCtl0 & 0x07);
	bool hFlip = (sprCtl0 & 0x20) != 0;
	bool vFlip = (sprCtl0 & 0x10) != 0;

	// === SPRCTL1 decoding (Handy/hardware bit layout) ===
	// Bit 0: StartLeft — quadrant start (left side)
	// Bit 1: StartUp — quadrant start (upper side)
	// Bit 3: ReloadPalette — 0 = reload from SCB, 1 = skip
	// Bits 5:4: ReloadDepth — 0=none, 1=size, 2=stretch, 3=tilt
	// Bit 7: Literal mode — raw pixel data
	bool startLeft      = (sprCtl1 & 0x01) != 0;
	bool startUp        = (sprCtl1 & 0x02) != 0;
	bool reloadPalette  = (sprCtl1 & 0x08) == 0; // Active low: 0=reload
	int  reloadDepth    = (sprCtl1 >> 4) & 0x03;
	bool literalMode    = (sprCtl1 & 0x80) != 0;

	// === SPRCOLL decoding ===
	uint8_t collNum      = sprColl & 0x0f;       // Collision number (0-15)
	bool    dontCollide  = (sprColl & 0x20) != 0; // Don't participate in collision

	// Determine which SCB fields to enable based on ReloadDepth
	bool enableStretch = false;
	bool enableTilt    = false;

	// Read always-present fields from SCB:
	uint16_t sprDataLine = ReadRam16(scbAddr + 5);   // Sprite data pointer
	_persistHpos = (int16_t)ReadRam16(scbAddr + 7);  // Horizontal start position
	_persistVpos = (int16_t)ReadRam16(scbAddr + 9);  // Vertical start position

	// Variable-length fields start at offset 11
	int scbOffset = 11;

	switch (reloadDepth) {
		case 3: // Size + stretch + tilt
			enableTilt = true;
			[[fallthrough]];
		case 2: // Size + stretch
			enableStretch = true;
			[[fallthrough]];
		case 1: // Size only
			_persistHsize = ReadRam16(scbAddr + scbOffset);
			_persistVsize = ReadRam16(scbAddr + scbOffset + 2);
			scbOffset += 4;
			if (reloadDepth >= 2) {
				_persistStretch = (int16_t)ReadRam16(scbAddr + scbOffset);
				scbOffset += 2;
			}
			if (reloadDepth >= 3) {
				_persistTilt = (int16_t)ReadRam16(scbAddr + scbOffset);
				scbOffset += 2;
			}
			break;
		default: // 0 = no optional fields
			break;
	}

	// Load palette remap if ReloadPalette is active
	if (reloadPalette) {
		for (int i = 0; i < 8; i++) {
			uint8_t byte = ReadRam(scbAddr + scbOffset + i);
			_penIndex[i * 2] = byte >> 4;
			_penIndex[i * 2 + 1] = byte & 0x0f;
		}
	}

	// === Quadrant rendering (matching Handy) ===
	// The Lynx renders each sprite in 4 quadrants: SE(0), NE(1), NW(2), SW(3)
	// starting from the quadrant specified by StartLeft/StartUp.
	//
	// Quadrant layout:    2 | 1
	//                    -------
	//                     3 | 0
	//
	// Each quadrant has independent hsign/vsign:
	//   Quadrant 0 (SE): hsign=+1, vsign=+1
	//   Quadrant 1 (NE): hsign=+1, vsign=-1
	//   Quadrant 2 (NW): hsign=-1, vsign=-1
	//   Quadrant 3 (SW): hsign=-1, vsign=+1
	//
	// H/V flip invert the signs.

	// Screen boundaries for clipping
	int screenHStart = (int)_state.HOffset;
	int screenHEnd   = (int)_state.HOffset + (int)LynxConstants::ScreenWidth;
	int screenVStart = (int)_state.VOffset;
	int screenVEnd   = (int)_state.VOffset + (int)LynxConstants::ScreenHeight;

	int worldHMid = screenHStart + (int)LynxConstants::ScreenWidth / 2;
	int worldVMid = screenVStart + (int)LynxConstants::ScreenHeight / 2;

	// Determine starting quadrant from SPRCTL1 bits 0-1
	int quadrant;
	if (startLeft) {
		quadrant = startUp ? 2 : 3;
	} else {
		quadrant = startUp ? 1 : 0;
	}

	// Superclipping: if sprite origin is off-screen, only render quadrants
	// that overlap the visible screen area
	int16_t sprH = _persistHpos;
	int16_t sprV = _persistVpos;
	bool superclip = (sprH < screenHStart || sprH >= screenHEnd ||
	                  sprV < screenVStart || sprV >= screenVEnd);

	// Track collision for this sprite
	bool everOnScreen = false;

	// Current sprite data pointer (advances through quadrants)
	uint16_t currentDataAddr = sprDataLine;

	// Loop over 4 quadrants
	for (int loop = 0; loop < 4; loop++) {
		// Calculate direction signs for this quadrant
		int hsign = (quadrant == 0 || quadrant == 1) ? 1 : -1;
		int vsign = (quadrant == 0 || quadrant == 3) ? 1 : -1;

		// H/V flip inverts the signs
		if (vFlip) vsign = -vsign;
		if (hFlip) hsign = -hsign;

		// Determine whether to render this quadrant
		bool render = false;

		if (superclip) {
			// Superclipping: only render if the screen overlaps this quadrant
			// relative to the sprite origin. Must account for h/v flip.
			int modquad = quadrant;
			static const int vquadflip[4] = { 1, 0, 3, 2 };
			static const int hquadflip[4] = { 3, 2, 1, 0 };
			if (vFlip) modquad = vquadflip[modquad];
			if (hFlip) modquad = hquadflip[modquad];

			switch (modquad) {
				case 0: // SE: screen to the right and below
					render = ((sprH < screenHEnd || sprH >= worldHMid) &&
					          (sprV < screenVEnd || sprV >= worldVMid));
					break;
				case 1: // NE: screen to the right and above
					render = ((sprH < screenHEnd || sprH >= worldHMid) &&
					          (sprV >= screenVStart || sprV <= worldVMid));
					break;
				case 2: // NW: screen to the left and above
					render = ((sprH >= screenHStart || sprH <= worldHMid) &&
					          (sprV >= screenVStart || sprV <= worldVMid));
					break;
				case 3: // SW: screen to the left and below
					render = ((sprH >= screenHStart || sprH <= worldHMid) &&
					          (sprV < screenVEnd || sprV >= worldVMid));
					break;
			}
		} else {
			render = true; // Origin on-screen: render all quadrants
		}

		if (render) {
			// Initialize vertical offset from sprite origin to screen
			int voff = (int)_persistVpos - screenVStart;

			// Reset tilt accumulator for each quadrant
			int32_t tiltAccum = 0;

			// Initialize size accumulators
			uint16_t vsizAccum = (vsign == 1) ? _state.VSizeOff : 0;
			uint16_t hsizAccum;

			// Quad offset fix: sprites drawn in a direction opposite to quad 0's
			// direction get offset by 1 pixel to prevent the squashed look
			int vquadoff = (loop == 0) ? vsign : ((vsign != ((quadrant == 0 || quadrant == 3) ? 1 : -1)) ? vsign : 0);
			int hquadoff_sign = 0; // Will be set on first quad

			// Working copies for this quadrant
			uint16_t hsize = _persistHsize;
			uint16_t vsize = _persistVsize;
			int16_t  qStretch = _persistStretch;
			int16_t  qTilt    = _persistTilt;
			int16_t  qHpos    = _persistHpos;

			// Render scanlines for this quadrant
			for (;;) {
				// Vertical scaling: accumulate vsize
				vsizAccum += vsize;
				int pixelHeight = (vsizAccum >> 8);
				vsizAccum &= 0x00ff; // Keep fractional part

				// Read line offset byte from sprite data
				uint8_t lineOffset = ReadRam(currentDataAddr);
				currentDataAddr++;

				if (lineOffset == 1) {
					// End of this quadrant — advance to next
					break;
				}
				if (lineOffset == 0) {
					// End of sprite — halt all quadrants
					loop = 4; // Will exit the outer for loop
					break;
				}

				// lineOffset gives total bytes for this line's data (including the offset byte)
				int lineDataBytes = lineOffset - 1;
				uint16_t lineEnd = currentDataAddr + lineDataBytes;

				// Decode pixel data for this line
				uint8_t pixelBuf[512];
				int pixelCount = DecodeSpriteLinePixels(currentDataAddr, lineEnd, bpp, literalMode, pixelBuf, 512);
				currentDataAddr = lineEnd;

				// Render this source line for pixelHeight destination lines
				for (int vloop = 0; vloop < pixelHeight; vloop++) {
					// Early bailout if off-screen in the render direction
					if (vsign == 1 && voff >= (int)LynxConstants::ScreenHeight) break;
					if (vsign == -1 && voff < 0) break;

					if (voff >= 0 && voff < (int)LynxConstants::ScreenHeight) {
						// Calculate horizontal start with tilt offset
						int hoff = (int)qHpos + (int)(tiltAccum >> 8) - screenHStart;

						// Initialize horizontal size accumulator
						hsizAccum = _state.HSizeOff;

						// Quad offset fix for horizontal
						if (loop == 0) hquadoff_sign = hsign;
						if (hsign != hquadoff_sign) hoff += hsign;

						// Render decoded pixels with horizontal scaling
						bool onscreen = false;
						for (int px = 0; px < pixelCount; px++) {
							uint8_t pixel = pixelBuf[px];

							// Horizontal scaling: accumulate hsize
							hsizAccum += hsize;
							int pixelWidth = (hsizAccum >> 8);
							hsizAccum &= 0x00ff;

							// Map through pen index table
							uint8_t penMapped = _penIndex[pixel & 0x0f];

							for (int hloop = 0; hloop < pixelWidth; hloop++) {
								if (hoff >= 0 && hoff < (int)LynxConstants::ScreenWidth) {
									if (pixel != 0) {
										WriteSpritePixel(hoff, voff, penMapped, collNum, spriteType);
									}
									onscreen = true;
									everOnScreen = true;
								} else {
									if (onscreen) break; // Went off-screen, skip rest
								}
								hoff += hsign;
							}
						}
					}

					voff += vsign;

					// Apply stretch and tilt per destination line (matching Handy)
					if (enableStretch) {
						hsize = (uint16_t)((int32_t)hsize + qStretch);
						// VStretch: also apply stretch to vsize per dest line
						if (_state.VStretch) {
							vsize = (uint16_t)((int32_t)vsize + qStretch);
						}
					}
					if (enableTilt) {
						tiltAccum += qTilt;
					}
				}
			}
		} else {
			// Skip through data to next quadrant
			// We need to consume data without rendering
			for (;;) {
				uint8_t lineOffset = ReadRam(currentDataAddr);
				currentDataAddr++;

				if (lineOffset == 1) break; // End of quadrant
				if (lineOffset == 0) {
					loop = 4; // End of sprite
					break;
				}
				// Skip over line data
				currentDataAddr += (lineOffset - 1);
			}
		}

		// Advance to next quadrant (wrapping 0-3)
		quadrant = (quadrant + 1) & 0x03;
	}

	// Write collision depositary (per Handy: stored in RAM at SCBAddr + COLLOFF)
	// TODO: Implement proper per-pixel collision tracking via COLLBAS buffer
	if (!dontCollide && !_state.NoCollide) {
		uint16_t collDep = (scbAddr + _state.CollOffset) & 0xFFFF;
		// Collision depositary: currently a placeholder, needs proper pixel-level
		// collision tracking against the collision buffer at COLLBAS
		WriteRam(collDep, 0);
	}

	// EVERON tracking: set high bit of collision byte if sprite was never on-screen
	if (_state.EverOn) {
		uint16_t collDep = (scbAddr + _state.CollOffset) & 0xFFFF;
		uint8_t colDat = ReadRam(collDep);
		if (!everOnScreen) {
			colDat |= 0x80;
		} else {
			colDat &= 0x7f;
		}
		WriteRam(collDep, colDat);
	}
}

/// <summary>
/// Decode one line of sprite pixel data.
///
/// The Lynx sprite engine supports two data formats (controlled by SPRCTL1 bit 7):
///
/// **Literal mode** (bit 7 = 1): All pixel data is raw linear bpp-wide values.
/// Each pixel is simply the next `bpp` bits from the data stream. No packet
/// structure, no run-length encoding.
///
/// **Packed mode** (bit 7 = 0): Uses a packetized format with RLE compression.
/// Each packet starts with a 1-bit flag:
///   - 1 = literal packet: 4-bit count, then count+1 literal pixel values (each bpp bits)
///   - 0 = packed (repeat) packet: 4-bit count, then one bpp-wide pixel repeated count+1 times
///     If count = 0 in a packed packet, it signals end-of-line.
///
/// In both modes, the line offset byte (already consumed by caller) gives the
/// total byte length of this line's data, limiting how many bits can be read.
/// </summary>
int LynxSuzy::DecodeSpriteLinePixels(uint16_t& dataAddr, uint16_t lineEnd, int bpp, bool literalMode, uint8_t* pixelBuf, int maxPixels) {
	int pixelCount = 0;
	uint8_t bppMask = (1 << bpp) - 1;

	// Bit-level reading state
	uint32_t shiftReg = 0;
	int shiftRegCount = 0;
	int totalBitsLeft = (int)(lineEnd - dataAddr) * 8;

	// Lambda to read N bits from the data stream
	auto getBits = [&](int bits) -> uint8_t {
		if (totalBitsLeft <= bits) {
			return 0; // No more data (matches Handy's <= check for demo006 fix)
		}

		// Refill shift register if needed
		while (shiftRegCount < bits && dataAddr < lineEnd) {
			shiftReg = (shiftReg << 8) | ReadRam(dataAddr);
			dataAddr++;
			shiftRegCount += 8;
		}

		if (shiftRegCount < bits) return 0;

		shiftRegCount -= bits;
		totalBitsLeft -= bits;
		return (uint8_t)((shiftReg >> shiftRegCount) & ((1 << bits) - 1));
	};

	if (literalMode) {
		// Literal mode: all pixels are raw bpp-wide values, no packet structure.
		// Total pixel count is (total data bits) / bpp.
		int totalPixels = totalBitsLeft / bpp;
		for (int i = 0; i < totalPixels && pixelCount < maxPixels; i++) {
			uint8_t pixel = getBits(bpp);
			pixelBuf[pixelCount++] = pixel;
			// In literal mode, a zero pixel as the very last pixel signals end of data
			// (matching Handy's line_abs_literal handling)
			if (pixelCount == totalPixels && pixel == 0) {
				pixelCount--; // Don't include trailing zero
				break;
			}
		}
	} else {
		// Packed mode: packetized data with literal and repeat packets
		while (totalBitsLeft > 0 && pixelCount < maxPixels) {
			// Read 1-bit literal flag
			uint8_t isLiteral = getBits(1);
			if (totalBitsLeft <= 0) break;

			// Read 4-bit count
			uint8_t count = getBits(4);

			if (!isLiteral && count == 0) {
				// Packed packet with count=0 = end of line
				break;
			}

			count++; // Actual count is stored count + 1

			if (isLiteral) {
				// Literal packet: read 'count' individual pixel values
				for (int i = 0; i < count && pixelCount < maxPixels; i++) {
					pixelBuf[pixelCount++] = getBits(bpp);
				}
			} else {
				// Packed (repeat) packet: read one pixel value, repeat 'count' times
				uint8_t pixel = getBits(bpp);
				for (int i = 0; i < count && pixelCount < maxPixels; i++) {
					pixelBuf[pixelCount++] = pixel;
				}
			}
		}
	}

	// Ensure dataAddr advances to lineEnd even if we stopped early
	dataAddr = lineEnd;
	return pixelCount;
}

__forceinline void LynxSuzy::WriteRam(uint16_t addr, uint8_t value) {
	if (_spriteProcessingActive) _spriteBusCycles++;
	_console->GetWorkRam()[addr & 0xFFFF] = value;
}

void LynxSuzy::WriteSpritePixel(int x, int y, uint8_t penIndex, uint8_t collNum, LynxSpriteType spriteType) {
	// Bounds check
	if (x < 0 || x >= (int)LynxConstants::ScreenWidth ||
		y < 0 || y >= (int)LynxConstants::ScreenHeight) {
		return;
	}

	// Transparent pixel (index 0) is never drawn
	if (penIndex == 0) {
		return;
	}

	// Non-collidable sprites draw pixels but don't update collision
	bool doCollision = (spriteType != LynxSpriteType::NonCollidable);

	// Background sprites only draw where the pixel is currently 0 (transparent)
	// and don't participate in collision
	if (spriteType == LynxSpriteType::Background) {
		doCollision = false;
	}

	// Shadow sprites: XOR the pixel value with the existing value
	bool isShadow = (spriteType == LynxSpriteType::NormalShadow ||
					 spriteType == LynxSpriteType::BoundaryShadow ||
					 spriteType == LynxSpriteType::XorShadow ||
					 spriteType == LynxSpriteType::Shadow);

	// Write pixel as 4bpp nibble into work RAM frame buffer
	// Use Suzy's VIDBAS register for sprite rendering target;
	// fall back to Mikey's display address for backward compatibility
	uint16_t dispAddr = _state.VideoBase ? _state.VideoBase
	                    : _console->GetMikey()->GetState().DisplayAddress;
	uint16_t byteAddr = dispAddr + y * LynxConstants::BytesPerScanline + (x >> 1);
	uint8_t byte = ReadRam(byteAddr);

	uint8_t existingPixel;
	uint8_t writePixel = penIndex & 0x0f;

	if (x & 1) {
		existingPixel = byte & 0x0f;
	} else {
		existingPixel = (byte >> 4) & 0x0f;
	}

	// Background sprite: only draw if existing pixel is transparent
	if (spriteType == LynxSpriteType::Background && existingPixel != 0) {
		return;
	}

	// Shadow/XOR sprites: XOR with existing pixel
	if (isShadow) {
		writePixel = existingPixel ^ writePixel;
	}

	if (x & 1) {
		byte = (byte & 0xF0) | writePixel;
	} else {
		byte = (byte & 0x0F) | (writePixel << 4);
	}
	WriteRam(byteAddr, byte);

	// Collision detection — mutual update algorithm
	// The collision buffer has 16 slots (indices 0-15).
	// When sprite A (collNum) draws over sprite B (existingPixel):
	//   1. Buffer[A] = max(Buffer[A], B) — A records that B collided with it
	//   2. Buffer[B] = max(Buffer[B], A) — B records that A collided with it
	// This ensures both sprites' entries reflect the highest-numbered collider.
	// The game reads Buffer[spriteN] to see what collided with sprite N.
	if (doCollision && collNum > 0 && collNum < LynxConstants::CollisionBufferSize) {
		// Read existing collision value for this sprite's collision number
		uint8_t existing = _state.CollisionBuffer[collNum];
		// Use the pen index of the existing pixel as the collider ID
		// Higher-numbered sprites have priority; only update if existingPixel > current value
		if (existingPixel > 0 && existingPixel > existing) {
			_state.CollisionBuffer[collNum] = existingPixel;
			// Set sticky sprite-to-sprite collision flag (cleared only by explicit write)
			_state.SpriteToSpriteCollision = true;
		}

		// Also update the colliding sprite's entry (mutual)
		if (existingPixel > 0 && existingPixel < LynxConstants::CollisionBufferSize) {
			uint8_t otherExisting = _state.CollisionBuffer[existingPixel];
			if (collNum > otherExisting) {
				_state.CollisionBuffer[existingPixel] = (uint8_t)collNum;
				_state.SpriteToSpriteCollision = true;
			}
		}
	}
}

uint8_t LynxSuzy::ReadRegister(uint8_t addr) {
	switch (addr) {
		// Sprite engine registers (FC80-FC83)
		case 0x80: // SPRCTL0
			return _state.SpriteControl0;
		case 0x81: // SPRCTL1
			return _state.SpriteControl1;
		case 0x82: // SPRCOLL
			return _state.SpriteInit; // TODO: separate SPRCOLL register
		case 0x83: // SPRINIT
			return _state.SpriteInit;

		// Suzy hardware revision (FC88)
		case 0x88: // SUZYHREV
			return 0x01; // Hardware revision = $01

		// Sprite engine status
		case 0x90: // SUZYBUSEN — sprite engine busy
			return _state.SpriteBusy ? 0x01 : 0x00;
		case 0x91: // SPRGO
			return _state.SpriteEnabled ? 0x01 : 0x00;
		case 0x92: // SPRSYS — system status (read)
			// Per Handy: bit0=SpriteWorking, bit1=StopOnCurrent, bit2=UnsafeAccess,
			// bit3=LeftHand, bit4=VStretch, bit5=LastCarry, bit6=MathOverflow,
			// bit7=MathInProgress
			return (_state.SpriteBusy ? 0x01 : 0x00) |              // Bit 0: sprite working
				(0) |                                                  // Bit 1: stop on current (TODO)
				(_state.UnsafeAccess ? 0x04 : 0x00) |                 // Bit 2: unsafe access
				(_state.LeftHand ? 0x08 : 0x00) |                     // Bit 3: left-handed
				(_state.VStretch ? 0x10 : 0x00) |                     // Bit 4: VStretch
				(_state.LastCarry ? 0x20 : 0x00) |                    // Bit 5: last carry
				(_state.MathOverflow ? 0x40 : 0x00) |                 // Bit 6: math overflow
				(_state.MathInProgress ? 0x80 : 0x00);                // Bit 7: math in progress

		// SCB address
		case 0x10: return (uint8_t)(_state.SCBAddress & 0xff);
		case 0x11: return (uint8_t)((_state.SCBAddress >> 8) & 0xff);

		// Math registers (read result)
		case 0x60: return (uint8_t)(_state.MathC & 0xff);
		case 0x61: return (uint8_t)((_state.MathC >> 8) & 0xff);
		case 0x62: return (uint8_t)(_state.MathD & 0xff);
		case 0x63: return (uint8_t)((_state.MathD >> 8) & 0xff);
		case 0x64: return (uint8_t)(_state.MathE & 0xff);
		case 0x65: return (uint8_t)((_state.MathE >> 8) & 0xff);
		case 0x66: return (uint8_t)(_state.MathF & 0xff);
		case 0x67: return (uint8_t)((_state.MathF >> 8) & 0xff);
		case 0x6c: return (uint8_t)(_state.MathG & 0xff);
		case 0x6d: return (uint8_t)((_state.MathG >> 8) & 0xff);
		case 0x6e: return (uint8_t)(_state.MathH & 0xff);
		case 0x6f: return (uint8_t)((_state.MathH >> 8) & 0xff);
		case 0x70: return (uint8_t)(_state.MathJ & 0xff);
		case 0x71: return (uint8_t)((_state.MathJ >> 8) & 0xff);
		case 0x72: return (uint8_t)(_state.MathK & 0xff);
		case 0x73: return (uint8_t)((_state.MathK >> 8) & 0xff);

		// Sprite rendering register reads
		case 0x04: return (uint8_t)(_state.HOffset & 0xff);
		case 0x05: return (uint8_t)((_state.HOffset >> 8) & 0xff);
		case 0x06: return (uint8_t)(_state.VOffset & 0xff);
		case 0x07: return (uint8_t)((_state.VOffset >> 8) & 0xff);
		case 0x08: return (uint8_t)(_state.VideoBase & 0xff);
		case 0x09: return (uint8_t)((_state.VideoBase >> 8) & 0xff);
		case 0x0a: return (uint8_t)(_state.CollisionBase & 0xff);
		case 0x0b: return (uint8_t)((_state.CollisionBase >> 8) & 0xff);

		// Collision depository: slots 0-3, 12-15
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x0c: case 0x0d: case 0x0e: case 0x0f:
			return _state.CollisionBuffer[addr];

		// Joystick / switches
		case 0xb0: // JOYSTICK
			return _state.Joystick;
		case 0xb1: // SWITCHES
			return _state.Switches;

		// Cart access registers (FCB2-FCB3)
		case 0xb2: // RCART0 — read from cart bank 0 (auto-increment)
			if (_cart) {
				_cart->SelectBank(0);
				return _cart->ReadData();
			}
			return 0xff;
		case 0xb3: // RCART1 — read from cart bank 1 (auto-increment)
			if (_cart) {
				_cart->SelectBank(1);
				return _cart->ReadData();
			}
			return 0xff;

		default:
			return 0xff;
	}
}

void LynxSuzy::WriteRegister(uint8_t addr, uint8_t value) {
	switch (addr) {
		// Sprite engine registers (FC80-FC83)
		case 0x80: // SPRCTL0
			_state.SpriteControl0 = value;
			break;
		case 0x81: // SPRCTL1
			_state.SpriteControl1 = value;
			break;
		case 0x82: // SPRCOLL
			// TODO: separate SPRCOLL register
			break;
		case 0x83: // SPRINIT
			_state.SpriteInit = value;
			break;

		// Sprite go
		case 0x91: // SPRGO — write 1 starts sprite engine
			_state.SpriteEnabled = (value & 0x01) != 0;
			_state.EverOn = (value & 0x04) != 0;    // Bit 2: EVERON tracking enable
			if (_state.SpriteEnabled) {
				ProcessSpriteChain();
			}
			break;
		case 0x92: // SPRSYS — write control bits
			_state.MathSign = (value & 0x80) != 0;       // Bit 7: signed math
			_state.MathAccumulate = (value & 0x40) != 0;  // Bit 6: accumulate mode
			_state.NoCollide = (value & 0x20) != 0;       // Bit 5: no collide
			_state.VStretch = (value & 0x10) != 0;        // Bit 4: vertical stretch
			_state.LeftHand = (value & 0x08) != 0;        // Bit 3: left-handed
			if (value & 0x04) _state.UnsafeAccess = false;            // Bit 2: clear unsafe access
			if (value & 0x02) _state.SpriteToSpriteCollision = false; // Bit 1: clear collision flag (spritestop)
			break;

		// Sprite rendering registers (FC04-FC2B)
		case 0x04: _state.HOffset = (_state.HOffset & (int16_t)0xff00) | value; break;
		case 0x05: _state.HOffset = (int16_t)((_state.HOffset & 0x00ff) | ((int16_t)value << 8)); break;
		case 0x06: _state.VOffset = (_state.VOffset & (int16_t)0xff00) | value; break;
		case 0x07: _state.VOffset = (int16_t)((_state.VOffset & 0x00ff) | ((int16_t)value << 8)); break;
		case 0x08: _state.VideoBase = (_state.VideoBase & 0xff00) | value; break;
		case 0x09: _state.VideoBase = (_state.VideoBase & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x0a: _state.CollisionBase = (_state.CollisionBase & 0xff00) | value; break;
		case 0x0b: _state.CollisionBase = (_state.CollisionBase & 0x00ff) | ((uint16_t)value << 8); break;

		// SCB address (FC10-FC11)
		case 0x10:
			_state.SCBAddress = (_state.SCBAddress & 0xff00) | value;
			break;
		case 0x11:
			_state.SCBAddress = (_state.SCBAddress & 0x00ff) | ((uint16_t)value << 8);
			break;

		// Collision offset and size offset registers
		case 0x24: _state.CollOffset = (_state.CollOffset & 0xff00) | value; break;
		case 0x25: _state.CollOffset = (_state.CollOffset & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x28: _state.HSizeOff = (_state.HSizeOff & 0xff00) | value; break;
		case 0x29: _state.HSizeOff = (_state.HSizeOff & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x2a: _state.VSizeOff = (_state.VSizeOff & 0xff00) | value; break;
		case 0x2b: _state.VSizeOff = (_state.VSizeOff & 0x00ff) | ((uint16_t)value << 8); break;

		// Math registers (write operands)
		case 0x60: _state.MathC = (_state.MathC & (int16_t)0xff00) | value; break;
		case 0x61: _state.MathC = (_state.MathC & 0x00ff) | ((int16_t)value << 8); break;
		case 0x62: _state.MathD = (_state.MathD & (int16_t)0xff00) | value; break;
		case 0x63: _state.MathD = (_state.MathD & 0x00ff) | ((int16_t)value << 8); break;
		case 0x64: _state.MathE = (_state.MathE & 0xff00) | value; break;
		case 0x65: _state.MathE = (_state.MathE & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x66: _state.MathF = (_state.MathF & 0xff00) | value; break;
		case 0x67:
			_state.MathF = (_state.MathF & 0x00ff) | ((uint16_t)value << 8);
			// Writing to MATHF high triggers multiply
			DoMultiply();
			break;

		// Writing to MATHK high triggers divide
		case 0x72: _state.MathK = (_state.MathK & 0xff00) | value; break;
		case 0x73:
			_state.MathK = (_state.MathK & 0x00ff) | ((uint16_t)value << 8);
			DoDivide();
			break;

		case 0x6c: _state.MathG = (_state.MathG & 0xff00) | value; break;
		case 0x6d: _state.MathG = (_state.MathG & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x6e: _state.MathH = (_state.MathH & 0xff00) | value; break;
		case 0x6f: _state.MathH = (_state.MathH & 0x00ff) | ((uint16_t)value << 8); break;
		case 0x70: _state.MathJ = (_state.MathJ & 0xff00) | value; break;
		case 0x71: _state.MathJ = (_state.MathJ & 0x00ff) | ((uint16_t)value << 8); break;

		// Collision depository writes: slots 0-3, 12-15 via registers
		// Note: offsets 0x04-0x0B are sprite rendering registers (HOFF, VOFF, VIDBAS, COLLBAS)
		// On real hardware, collision data is stored in RAM at SCBAddr+COLLOFF, not
		// in the register space. This is a simplification for now.
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x0c: case 0x0d: case 0x0e: case 0x0f:
			_state.CollisionBuffer[addr] = value;
			break;

		// Cart access registers (FCB2-FCB3)
		case 0xb2: // RCART0 — read/write cart bank 0
			if (_cart) _cart->SetAddressLow(value);
			break;
		case 0xb3: // RCART1 — read/write cart bank 1
			if (_cart) _cart->SetAddressLow(value);
			break;

		default:
			break;
	}
}

void LynxSuzy::Serialize(Serializer& s) {
	// Sprite engine
	SV(_state.SCBAddress);
	SV(_state.SpriteControl0);
	SV(_state.SpriteControl1);
	SV(_state.SpriteInit);
	SV(_state.SpriteBusy);
	SV(_state.SpriteEnabled);

	// Math
	SV(_state.MathA);
	SV(_state.MathB);
	SV(_state.MathC);
	SV(_state.MathD);
	SV(_state.MathE);
	SV(_state.MathF);
	SV(_state.MathG);
	SV(_state.MathH);
	SV(_state.MathJ);
	SV(_state.MathK);
	SV(_state.MathM);
	SV(_state.MathN);
	SV(_state.MathSign);
	SV(_state.MathAccumulate);
	SV(_state.MathInProgress);
	SV(_state.MathOverflow);
	SV(_state.LastCarry);
	SV(_state.UnsafeAccess);
	SV(_state.SpriteToSpriteCollision);
	SV(_state.VStretch);
	SV(_state.LeftHand);

	// Collision
	SVArray(_state.CollisionBuffer, LynxConstants::CollisionBufferSize);

	// Sprite rendering registers
	SV(_state.HOffset);
	SV(_state.VOffset);
	SV(_state.VideoBase);
	SV(_state.CollisionBase);
	SV(_state.CollOffset);
	SV(_state.HSizeOff);
	SV(_state.VSizeOff);
	SV(_state.EverOn);
	SV(_state.NoCollide);

	// Input
	SV(_state.Joystick);
	SV(_state.Switches);

	// Pen index remap table
	SVArray(_penIndex, 16);

	// Persistent SCB fields (reused across sprites when reload flags clear)
	SV(_persistHpos);
	SV(_persistVpos);
	SV(_persistHsize);
	SV(_persistVsize);
	SV(_persistStretch);
	SV(_persistTilt);
}
