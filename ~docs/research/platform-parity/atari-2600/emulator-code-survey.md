# Atari 2600 Emulator Code Survey

## Scope

Compare architecture signals from three mature implementations to identify safe adoption patterns for Nexen.

## Stella

Observed characteristics:

- Monolithic but highly detailed TIA implementation with explicit delayed event and frame management concepts.
- Tight coupling between TIA updates and CPU/system cycle state.
- Mature RIOT model with extensive debugger-facing introspection.

References:

- [https://github.com/stella-emu/stella/blob/main/src/emucore/tia/TIA.cxx](https://github.com/stella-emu/stella/blob/main/src/emucore/tia/TIA.cxx)
- [https://github.com/stella-emu/stella/blob/main/src/emucore/tia/TIA.hxx](https://github.com/stella-emu/stella/blob/main/src/emucore/tia/TIA.hxx)
- [https://github.com/stella-emu/stella/blob/main/src/emucore/M6532.hxx](https://github.com/stella-emu/stella/blob/main/src/emucore/M6532.hxx)

## Gopher2600

Observed characteristics:

- Clear package-level separation (TIA, RIOT, memory bus layers).
- Explicit step model and strong inline design commentary.
- Mapper implementations with practical compatibility notes.

References:

- [https://github.com/jetsetilly/gopher2600/blob/main/hardware/tia/tia.go](https://github.com/jetsetilly/gopher2600/blob/main/hardware/tia/tia.go)
- [https://github.com/jetsetilly/gopher2600/blob/main/hardware/riot/riot.go](https://github.com/jetsetilly/gopher2600/blob/main/hardware/riot/riot.go)
- [https://github.com/jetsetilly/gopher2600/blob/main/hardware/step.go](https://github.com/jetsetilly/gopher2600/blob/main/hardware/step.go)
- [https://github.com/jetsetilly/gopher2600/tree/main/hardware/memory/cartridge](https://github.com/jetsetilly/gopher2600/tree/main/hardware/memory/cartridge)

## ares

Observed characteristics:

- Componentized CPU/thread structure compatible with multi-system architecture goals.
- Useful as a minimal reference for integration boundaries.

References:

- [https://github.com/ares-emulator/ares/blob/main/ares/a26/cpu/cpu.cpp](https://github.com/ares-emulator/ares/blob/main/ares/a26/cpu/cpu.cpp)
- [https://github.com/ares-emulator/ares/blob/main/ares/a26/cpu/cpu.hpp](https://github.com/ares-emulator/ares/blob/main/ares/a26/cpu/cpu.hpp)

## Recommendations For Nexen

1. Adopt explicit cycle-step contracts similar to Gopher2600 for clarity and testability.
2. Borrow detailed TIA edge-case validation ideas from Stella.
3. Keep component boundaries close to existing Nexen architecture, using ares as a structural reference.
