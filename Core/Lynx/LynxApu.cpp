#include "pch.h"
#include "Lynx/LynxApu.h"
#include "Lynx/LynxConsole.h"
#include "Shared/Emulator.h"
#include "Shared/Audio/SoundMixer.h"
#include "Utilities/Serializer.h"

LynxApu::LynxApu(Emulator* emu, LynxConsole* console)
	: _emu(emu), _console(console) {
	_soundMixer = emu->GetSoundMixer();
	_soundBuffer = std::make_unique<int16_t[]>(MaxSamples * 2); // Stereo interleaved
}

void LynxApu::Init() {
	_state = {};
	_sampleCount = 0;
	_clockAccumulator = 0;

	// Initialize channels to default state
	for (int i = 0; i < 4; i++) {
		_state.Channels[i] = {};
		_state.Channels[i].ShiftRegister = 0x001; // Non-zero initial LFSR
	}
}

void LynxApu::Tick() {
	_clockAccumulator++;

	// Generate one audio sample every ClocksPerSample master clocks
	if (_clockAccumulator >= ClocksPerSample) {
		_clockAccumulator -= ClocksPerSample;

		// Clock each channel's timer
		for (int ch = 0; ch < 4; ch++) {
			ClockChannel(ch);
		}

		MixOutput();
	}
}

void LynxApu::ClockChannel(int ch) {
	LynxAudioChannelState& channel = _state.Channels[ch];

	if (!channel.Enabled) {
		return;
	}

	// Decrement counter
	if (channel.Counter > 0) {
		channel.Counter--;
		return;
	}

	// Counter underflow — reload and clock LFSR
	channel.Counter = channel.BackupValue;

	// Clock the 12-bit LFSR (linear feedback shift register)
	// Feedback taps are selected by FeedbackEnable register
	// The feedback value is XOR of selected shift register bits
	uint16_t sr = channel.ShiftRegister;
	uint8_t feedback = 0;

	// Each bit in FeedbackEnable selects a tap on the shift register
	// Taps: bits 0,1,2,3,4,5,7,10 of the shift register correspond to
	// bits 0,1,2,3,4,5,6,7 of the feedback enable register
	static constexpr uint8_t tapBits[] = { 0, 1, 2, 3, 4, 5, 7, 10 };
	for (int i = 0; i < 8; i++) {
		if (channel.FeedbackEnable & (1 << i)) {
			feedback ^= ((sr >> tapBits[i]) & 1);
		}
	}

	// Shift register: shift right, new bit enters at bit 11
	sr = (sr >> 1) | (feedback << 11);
	channel.ShiftRegister = sr & 0x0fff; // Keep 12 bits

	// Output value depends on the low bit of the shift register
	// In integration mode, the output accumulates
	if (channel.Integrate) {
		channel.Output += (sr & 1) ? channel.Volume : (int8_t)(-channel.Volume);
		// Clamp to signed 8-bit range
		if (channel.Output > 127) channel.Output = 127;
		if (channel.Output < -128) channel.Output = -128;
	} else {
		channel.Output = (sr & 1) ? channel.Volume : (int8_t)(-channel.Volume);
	}
}

void LynxApu::MixOutput() {
	int32_t leftSum = 0;
	int32_t rightSum = 0;

	for (int ch = 0; ch < 4; ch++) {
		LynxAudioChannelState& channel = _state.Channels[ch];
		if (!channel.Enabled) {
			continue;
		}

		int32_t sample = channel.Output;

		// Apply per-channel stereo attenuation (4-bit, 0-15)
		int32_t left = (sample * channel.LeftAtten) >> 2;
		int32_t right = (sample * channel.RightAtten) >> 2;

		leftSum += left;
		rightSum += right;
	}

	// Apply master volume (simple scaling)
	// MasterVolume is 0-255, treat as 0-1 fraction
	leftSum = (leftSum * (_state.MasterVolume + 1)) >> 4;
	rightSum = (rightSum * (_state.MasterVolume + 1)) >> 4;

	// If not stereo, output mono on both channels
	if (!_state.StereoEnabled) {
		int32_t mono = (leftSum + rightSum) / 2;
		leftSum = mono;
		rightSum = mono;
	}

	// Scale to 16-bit range
	leftSum = std::clamp(leftSum * 64, (int32_t)INT16_MIN, (int32_t)INT16_MAX);
	rightSum = std::clamp(rightSum * 64, (int32_t)INT16_MIN, (int32_t)INT16_MAX);

	_soundBuffer[_sampleCount * 2] = (int16_t)leftSum;
	_soundBuffer[_sampleCount * 2 + 1] = (int16_t)rightSum;
	_sampleCount++;

	if (_sampleCount >= MaxSamples) {
		PlayQueuedAudio();
	}
}

void LynxApu::PlayQueuedAudio() {
	_soundMixer->PlayAudioBuffer(_soundBuffer.get(), _sampleCount, SampleRate);
	_sampleCount = 0;
}

void LynxApu::EndFrame() {
	if (_sampleCount > 0) {
		PlayQueuedAudio();
	}
}

uint8_t LynxApu::ReadRegister(uint8_t addr) {
	// $FD20-$FD3F: Channel registers (4 channels × 8 bytes)
	if (addr < 0x20) {
		int ch = (addr >> 3) & 0x03;
		int reg = addr & 0x07;
		LynxAudioChannelState& channel = _state.Channels[ch];

		switch (reg) {
			case 0: return channel.Volume;
			case 1: return channel.FeedbackEnable;
			case 2: return (uint8_t)channel.Output;
			case 3: return (uint8_t)(channel.ShiftRegister & 0xff);
			case 4: return (uint8_t)(channel.ShiftRegister >> 8);
			case 5: return channel.BackupValue;
			case 6: return channel.Control;
			case 7: return channel.Counter;
		}
	}

	// $FD40-$FD47: Stereo attenuation (4 channels × 2 bytes L/R)
	if (addr >= 0x20 && addr < 0x28) {
		int ch = (addr - 0x20) >> 1;
		if (addr & 1) {
			return _state.Channels[ch].RightAtten;
		} else {
			return _state.Channels[ch].LeftAtten;
		}
	}

	// $FD50: Master volume / attenuation
	if (addr == 0x30) {
		return _state.MasterVolume;
	}

	return 0;
}

void LynxApu::WriteRegister(uint8_t addr, uint8_t value) {
	// $FD20-$FD3F: Channel registers
	if (addr < 0x20) {
		int ch = (addr >> 3) & 0x03;
		int reg = addr & 0x07;
		LynxAudioChannelState& channel = _state.Channels[ch];

		switch (reg) {
			case 0: channel.Volume = value; break; // Full 8-bit volume (7-bit magnitude)
			case 1: channel.FeedbackEnable = value; break;
			case 2: channel.Output = (int8_t)value; break;
			case 3: channel.ShiftRegister = (channel.ShiftRegister & 0xf00) | value; break;
			case 4: channel.ShiftRegister = (channel.ShiftRegister & 0x0ff) | ((value & 0x0f) << 8); break;
			case 5: channel.BackupValue = value; break;
			case 6:
				channel.Control = value;
				channel.Enabled = (value & 0x80) != 0;
				channel.Integrate = (value & 0x20) != 0;
				break;
			case 7: channel.Counter = value; break;
		}
		return;
	}

	// $FD40-$FD47: Stereo attenuation
	if (addr >= 0x20 && addr < 0x28) {
		int ch = (addr - 0x20) >> 1;
		if (addr & 1) {
			_state.Channels[ch].RightAtten = value & 0x0f;
		} else {
			_state.Channels[ch].LeftAtten = value & 0x0f;
		}
		return;
	}

	// $FD50: Master volume
	if (addr == 0x30) {
		_state.MasterVolume = value;
		return;
	}
}

void LynxApu::Serialize(Serializer& s) {
	for (int i = 0; i < 4; i++) {
		SVI(_state.Channels[i].Volume);
		SVI(_state.Channels[i].FeedbackEnable);
		SVI(_state.Channels[i].Output);
		SVI(_state.Channels[i].ShiftRegister);
		SVI(_state.Channels[i].BackupValue);
		SVI(_state.Channels[i].Control);
		SVI(_state.Channels[i].Counter);
		SVI(_state.Channels[i].LeftAtten);
		SVI(_state.Channels[i].RightAtten);
		SVI(_state.Channels[i].Integrate);
		SVI(_state.Channels[i].Enabled);
	}
	SV(_state.MasterVolume);
	SV(_state.StereoEnabled);

	SV(_sampleCount);
	SV(_clockAccumulator);
}
