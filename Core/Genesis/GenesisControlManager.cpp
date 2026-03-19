#include "pch.h"
#include "Genesis/GenesisControlManager.h"
#include "Genesis/GenesisConsole.h"
#include "Genesis/Input/GenesisController.h"
#include "Shared/Emulator.h"
#include "Shared/EmuSettings.h"
#include "Utilities/Serializer.h"

GenesisControlManager::GenesisControlManager(Emulator* emu, GenesisConsole* console)
	: BaseControlManager(emu, CpuType::Genesis) {
	_console = console;
}

shared_ptr<BaseControlDevice> GenesisControlManager::CreateControllerDevice(ControllerType type, uint8_t port) {
	shared_ptr<BaseControlDevice> device;

	KeyMappingSet keys;
	switch (port) {
		default:
		case 0:
			keys = _emu->GetSettings()->GetGenesisConfig().Port1.Keys;
			break;
		case 1:
			keys = _emu->GetSettings()->GetGenesisConfig().Port2.Keys;
			break;
	}

	switch (type) {
		default:
		case ControllerType::None:
			break;
		case ControllerType::GenesisController:
			device = std::make_unique<GenesisController>(_emu, port, keys);
			break;
	}

	return device;
}

void GenesisControlManager::UpdateControlDevices() {
	GenesisConfig& cfg = _emu->GetSettings()->GetGenesisConfig();
	if (_emu->GetSettings()->IsEqual(_prevConfig, cfg) && _controlDevices.size() > 0) {
		return;
	}

	auto lock = _deviceLock.AcquireSafe();
	ClearDevices();

	for (int i = 0; i < 2; i++) {
		shared_ptr<BaseControlDevice> device = CreateControllerDevice(
			i == 0 ? cfg.Port1.Type : cfg.Port2.Type, i
		);
		if (device) {
			RegisterControlDevice(device);
		}
	}

	_prevConfig = cfg;
}

uint8_t GenesisControlManager::ReadDataPort(uint8_t port) {
	SetInputReadFlag();

	if (port > 1) {
		return 0xff;
	}

	uint8_t value = 0;

	shared_ptr<BaseControlDevice> device = GetControlDevice(port);
	if (!device || !device->IsConnected()) {
		return 0x7f;
	}

	bool thHigh = (_dataPortWrite[port] & 0x40) != 0;

	if (thHigh) {
		// TH = 1: Up, Down, Left, Right, B, C
		value |= device->IsPressed(GenesisController::Buttons::Up)    ? 0 : 0x01;
		value |= device->IsPressed(GenesisController::Buttons::Down)  ? 0 : 0x02;
		value |= device->IsPressed(GenesisController::Buttons::Left)  ? 0 : 0x04;
		value |= device->IsPressed(GenesisController::Buttons::Right) ? 0 : 0x08;
		value |= device->IsPressed(GenesisController::Buttons::B)     ? 0 : 0x10;
		value |= device->IsPressed(GenesisController::Buttons::C)     ? 0 : 0x20;
		value |= 0x40; // TH readback
	} else {
		// TH = 0: Up, Down, 0, 0, A, Start
		value |= device->IsPressed(GenesisController::Buttons::Up)    ? 0 : 0x01;
		value |= device->IsPressed(GenesisController::Buttons::Down)  ? 0 : 0x02;
		value |= device->IsPressed(GenesisController::Buttons::A)     ? 0 : 0x10;
		value |= device->IsPressed(GenesisController::Buttons::Start) ? 0 : 0x20;
		// Bits 2,3 are 0 when TH=0 (active low, but always pressed → 0)
	}

	return value;
}

void GenesisControlManager::WriteDataPort(uint8_t port, uint8_t value) {
	if (port > 1) {
		return;
	}
	_dataPortWrite[port] = value;
}

void GenesisControlManager::Serialize(Serializer& s) {
	BaseControlManager::Serialize(s);
	SVArray(_dataPortWrite, 2);
	SVArray(_thState, 2);
	SVArray(_thCount, 2);
}
