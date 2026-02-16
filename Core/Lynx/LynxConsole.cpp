#include "pch.h"
#include "Lynx/LynxConsole.h"
#include "Lynx/LynxTypes.h"
#include "Shared/Emulator.h"
#include "Shared/EmuSettings.h"
#include "Shared/MessageManager.h"
#include "Shared/BatteryManager.h"
#include "Utilities/Serializer.h"
#include "Utilities/VirtualFile.h"

// Component stubs — will be replaced as each component is implemented
// #include "Lynx/LynxCpu.h"
// #include "Lynx/LynxMikey.h"
// #include "Lynx/LynxSuzy.h"
// #include "Lynx/LynxMemoryManager.h"
// #include "Lynx/LynxCart.h"
// #include "Lynx/LynxControlManager.h"

LynxConsole::LynxConsole(Emulator* emu) {
	_emu = emu;
}

LynxConsole::~LynxConsole() {
	delete[] _workRam;
	delete[] _prgRom;
	delete[] _bootRom;
	delete[] _saveRam;
}

LoadRomResult LynxConsole::LoadRom(VirtualFile& romFile) {
	vector<uint8_t> romData;
	romFile.ReadFile(romData);

	if (romData.size() < 64) {
		return LoadRomResult::Failure;
	}

	// Check for LNX header: magic bytes "LYNX" at offset 0
	bool hasLnxHeader = (romData[0] == 'L' && romData[1] == 'Y' && romData[2] == 'N' && romData[3] == 'X');
	uint32_t romOffset = 0;

	MessageManager::Log("------------------------------");

	if (hasLnxHeader) {
		// LNX header is 64 bytes
		// Bytes 0-3: "LYNX" magic
		// Bytes 4-5: Bank 0 page size (little-endian, in 256-byte pages)
		// Bytes 6-7: Bank 1 page size (little-endian, in 256-byte pages)
		// Bytes 8-9: Version (usually 1)
		// Bytes 10-41: Cart name (32 bytes, null-terminated)
		// Bytes 42-57: Manufacturer name (16 bytes, null-terminated)
		// Byte 58: Rotation (0=none, 1=left, 2=right)
		// Bytes 59-63: Reserved

		uint16_t bank0Pages = romData[4] | (romData[5] << 8);
		uint16_t bank1Pages = romData[6] | (romData[7] << 8);
		uint16_t version = romData[8] | (romData[9] << 8);

		string cartName(reinterpret_cast<char*>(&romData[10]), 32);
		cartName = cartName.c_str(); // Trim at null terminator
		string manufacturer(reinterpret_cast<char*>(&romData[42]), 16);
		manufacturer = manufacturer.c_str();

		uint8_t rotation = romData[58];
		switch (rotation) {
			case 1: _rotation = LynxRotation::Left; break;
			case 2: _rotation = LynxRotation::Right; break;
			default: _rotation = LynxRotation::None; break;
		}

		MessageManager::Log("LNX Header:");
		MessageManager::Log(std::format("  Cart Name: {}", cartName));
		MessageManager::Log(std::format("  Manufacturer: {}", manufacturer));
		MessageManager::Log(std::format("  Version: {}", version));
		MessageManager::Log(std::format("  Bank 0 Pages: {} ({} KB)", bank0Pages, bank0Pages * 256 / 1024));
		MessageManager::Log(std::format("  Bank 1 Pages: {} ({} KB)", bank1Pages, bank1Pages * 256 / 1024));
		MessageManager::Log(std::format("  Rotation: {}", rotation == 0 ? "None" : (rotation == 1 ? "Left" : "Right")));

		romOffset = 64; // Skip header
	} else {
		// Headerless .o format — raw ROM data
		MessageManager::Log("Headerless ROM (raw .o format)");
		_rotation = LynxRotation::None;
	}

	// Extract ROM data
	_prgRomSize = (uint32_t)(romData.size() - romOffset);
	if (_prgRomSize == 0) {
		return LoadRomResult::Failure;
	}

	_prgRom = new uint8_t[_prgRomSize];
	memcpy(_prgRom, romData.data() + romOffset, _prgRomSize);
	_emu->RegisterMemory(MemoryType::LynxPrgRom, _prgRom, _prgRomSize);

	MessageManager::Log(std::format("ROM Size: {} KB", _prgRomSize / 1024));

	// Allocate work RAM (64 KB)
	_workRamSize = LynxConstants::WorkRamSize;
	_workRam = new uint8_t[_workRamSize];
	memset(_workRam, 0, _workRamSize);
	_emu->RegisterMemory(MemoryType::LynxWorkRam, _workRam, _workRamSize);

	// Boot ROM — optional, loaded from firmware
	// TODO: Load boot ROM via FirmwareHelper when implemented
	// For now, no boot ROM
	_bootRomSize = 0;
	_bootRom = nullptr;

	// Save RAM (EEPROM) — size determined by ROM database or header
	// TODO: Determine EEPROM size from cart info
	_saveRamSize = 0;
	_saveRam = nullptr;

	MessageManager::Log(std::format("Work RAM: {} KB", _workRamSize / 1024));
	MessageManager::Log("------------------------------");

	// TODO: Create components once they are implemented
	// _controlManager.reset(new LynxControlManager(_emu, this));
	// _memoryManager.reset(new LynxMemoryManager());
	// _cart.reset(new LynxCart());
	// _mikey.reset(new LynxMikey(_emu, this));
	// _suzy.reset(new LynxSuzy(_emu, this));
	// _cpu.reset(new LynxCpu(_emu, this, _memoryManager.get()));
	// Wire components together via Init() calls
	// LoadBattery();

	// Initialize frame buffer to black
	memset(_frameBuffer, 0, sizeof(_frameBuffer));

	return LoadRomResult::Success;
}

void LynxConsole::RunFrame() {
	// TODO: Implement when LynxCpu and LynxMikey are available
	// uint32_t frameCount = _mikey->GetFrameCount();
	// while (frameCount == _mikey->GetFrameCount()) {
	//     _cpu->Exec();
	// }
}

void LynxConsole::Reset() {
	// The Lynx has no reset button — behave like power cycle
	_emu->ReloadRom(true);
}

void LynxConsole::SaveBattery() {
	if (_saveRam && _saveRamSize > 0) {
		_emu->GetBatteryManager()->SaveBattery(".sav", std::span<const uint8_t>(_saveRam, _saveRamSize));
	}
}

void LynxConsole::LoadBattery() {
	if (_saveRam && _saveRamSize > 0) {
		_emu->GetBatteryManager()->LoadBattery(".sav", std::span<uint8_t>(_saveRam, _saveRamSize));
	}
}

BaseControlManager* LynxConsole::GetControlManager() {
	// TODO: Return _controlManager.get() when LynxControlManager is implemented
	return nullptr;
}

ConsoleRegion LynxConsole::GetRegion() {
	return ConsoleRegion::Ntsc;
}

ConsoleType LynxConsole::GetConsoleType() {
	return ConsoleType::Lynx;
}

vector<CpuType> LynxConsole::GetCpuTypes() {
	return { CpuType::Lynx };
}

uint64_t LynxConsole::GetMasterClock() {
	// TODO: Return actual CPU cycle count when LynxCpu is implemented
	return 0;
}

uint32_t LynxConsole::GetMasterClockRate() {
	return LynxConstants::MasterClockRate;
}

double LynxConsole::GetFps() {
	return LynxConstants::Fps;
}

BaseVideoFilter* LynxConsole::GetVideoFilter(bool getDefaultFilter) {
	// TODO: Implement LynxDefaultVideoFilter
	return nullptr;
}

PpuFrameInfo LynxConsole::GetPpuFrame() {
	PpuFrameInfo frame = {};
	frame.FirstScanline = 0;
	frame.FrameCount = 0; // TODO: Get from Mikey
	frame.Width = LynxConstants::ScreenWidth;
	frame.Height = LynxConstants::ScreenHeight;
	frame.ScanlineCount = LynxConstants::ScanlineCount;
	frame.CycleCount = LynxConstants::CpuCyclesPerScanline;
	frame.FrameBufferSize = frame.Width * frame.Height * sizeof(uint32_t);
	frame.FrameBuffer = (uint8_t*)_frameBuffer;
	return frame;
}

RomFormat LynxConsole::GetRomFormat() {
	return RomFormat::Lynx;
}

AudioTrackInfo LynxConsole::GetAudioTrackInfo() {
	return {};
}

void LynxConsole::ProcessAudioPlayerAction(AudioPlayerActionParams p) {
	// Not applicable for Lynx
}

AddressInfo LynxConsole::GetAbsoluteAddress(AddressInfo& relAddress) {
	// TODO: Delegate to LynxMemoryManager when implemented
	return { -1, MemoryType::None };
}

AddressInfo LynxConsole::GetRelativeAddress(AddressInfo& absAddress, CpuType cpuType) {
	// TODO: Delegate to LynxMemoryManager when implemented
	return { -1, MemoryType::None };
}

void LynxConsole::GetConsoleState(BaseState& state, ConsoleType consoleType) {
	(LynxState&)state = GetState();
}

LynxState LynxConsole::GetState() {
	LynxState state = {};
	state.Model = _model;
	// TODO: Fill sub-states from components when implemented
	// state.Cpu = _cpu->GetState();
	// state.Mikey = _mikey->GetState();
	// state.Ppu = _mikey->GetPpuState();
	// state.Apu = _mikey->GetApuState();
	// state.Suzy = _suzy->GetState();
	// state.MemoryManager = _memoryManager->GetState();
	// state.Cart = _cart->GetCartState();
	// state.ControlManager = _controlManager->GetState();
	return state;
}

void LynxConsole::Serialize(Serializer& s) {
	SV(_model);
	SV(_rotation);

	// TODO: Serialize components when implemented
	// SV(_cpu);
	// SV(_mikey);
	// SV(_suzy);
	// SV(_cart);
	// SV(_memoryManager);
	// SV(_controlManager);

	SVArray(_workRam, _workRamSize);
	if (_saveRam && _saveRamSize > 0) {
		SVArray(_saveRam, _saveRamSize);
	}
}
