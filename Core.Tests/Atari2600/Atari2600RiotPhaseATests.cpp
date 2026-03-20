#include "pch.h"
#include "Shared/Emulator.h"
#include "Atari2600/Atari2600Console.h"

namespace {
	TEST(Atari2600RiotPhaseATests, PortDirectionMasksOutputAndInputReads) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.DebugWriteCartridge(0x0282, 0x0F); // DDRA: low nibble output
		console.DebugWriteCartridge(0x0280, 0xA5); // SWCHA output latch

		EXPECT_EQ(console.DebugReadCartridge(0x0282), 0x0Fu);
		EXPECT_EQ(console.DebugReadCartridge(0x0280), 0x05u);

		Atari2600RiotState riotState = console.GetRiotState();
		EXPECT_EQ(riotState.PortA, 0xA5u);
		EXPECT_EQ(riotState.PortADirection, 0x0Fu);
	}

	TEST(Atari2600RiotPhaseATests, TimerDividerAndUnderflowEdgesAreDeterministic) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.DebugWriteCartridge(0x0284, 0x02); // divider=1, timer=2

		console.StepCpuCycles(3);
		Atari2600RiotState firstUnderflow = console.GetRiotState();
		EXPECT_TRUE(firstUnderflow.TimerUnderflow);
		EXPECT_TRUE(firstUnderflow.InterruptFlag);
		EXPECT_EQ(firstUnderflow.InterruptEdgeCount, 1u);

		console.StepCpuCycles(256);
		Atari2600RiotState secondUnderflow = console.GetRiotState();
		EXPECT_EQ(secondUnderflow.InterruptEdgeCount, 2u);

		console.Reset();
		console.DebugWriteCartridge(0x0285, 0x01); // divider=8, timer=1
		console.StepCpuCycles(15);
		EXPECT_FALSE(console.GetRiotState().TimerUnderflow);
		console.StepCpuCycles(1);
		EXPECT_TRUE(console.GetRiotState().TimerUnderflow);
	}

	TEST(Atari2600RiotPhaseATests, InstatReadClearsInterruptLatchBeforeIntimClearsUnderflowLatch) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.DebugWriteCartridge(0x0284, 0x00); // divider=1, timer=0 (underflow on next tick)
		console.StepCpuCycles(1);

		EXPECT_EQ(console.DebugReadCartridge(0x0286), 1u); // instat first read
		EXPECT_EQ(console.DebugReadCartridge(0x0286), 0u); // instat clears interrupt latch
		EXPECT_EQ(console.DebugReadCartridge(0x0287), 1u); // underflow still set until intim read
		EXPECT_EQ(console.DebugReadCartridge(0x0284), 0xFFu); // intim read clears underflow latch
		EXPECT_EQ(console.DebugReadCartridge(0x0287), 0u);

		Atari2600RiotState afterReads = console.GetRiotState();
		EXPECT_FALSE(afterReads.InterruptFlag);
		EXPECT_FALSE(afterReads.TimerUnderflow);
		EXPECT_EQ(afterReads.InterruptEdgeCount, 1u);
	}

	TEST(Atari2600RiotPhaseATests, IntimReadFirstKeepsInterruptLatchSetUntilInstatRead) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.DebugWriteCartridge(0x0284, 0x00); // divider=1, timer=0 (underflow on next tick)
		console.StepCpuCycles(1);

		EXPECT_EQ(console.DebugReadCartridge(0x0284), 0xFFu); // intim clears underflow latch
		EXPECT_EQ(console.DebugReadCartridge(0x0287), 0u);
		EXPECT_EQ(console.DebugReadCartridge(0x0286), 1u);
		EXPECT_EQ(console.DebugReadCartridge(0x0286), 0u);

		Atari2600RiotState afterReads = console.GetRiotState();
		EXPECT_FALSE(afterReads.InterruptFlag);
		EXPECT_FALSE(afterReads.TimerUnderflow);
		EXPECT_EQ(afterReads.InterruptEdgeCount, 1u);
	}

	TEST(Atari2600RiotPhaseATests, TimerReconfigureClearsStatusAfterReadSequence) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.DebugWriteCartridge(0x0284, 0x00); // underflow on next step
		console.StepCpuCycles(1);

		// Read status first to lock deterministic read-clear ordering, then reconfigure timer.
		EXPECT_EQ(console.DebugReadCartridge(0x0286), 1u);
		EXPECT_EQ(console.DebugReadCartridge(0x0284), 0xFFu);
		console.DebugWriteCartridge(0x0284, 0x05);

		Atari2600RiotState state = console.GetRiotState();
		EXPECT_FALSE(state.InterruptFlag);
		EXPECT_FALSE(state.TimerUnderflow);
		EXPECT_EQ(state.Timer, 0x0005u);
		EXPECT_EQ(state.TimerDivider, 1u);
		EXPECT_EQ(state.InterruptEdgeCount, 1u);
	}

	TEST(Atari2600RiotPhaseATests, ReadClearOrderingIsDeterministicAcrossRuns) {
		auto runSequence = []() {
			Emulator emu;
			Atari2600Console console(&emu);

			console.Reset();
			console.DebugWriteCartridge(0x0284, 0x00);
			console.StepCpuCycles(1);

			std::array<uint8_t, 6> sequence = {};
			sequence[0] = console.DebugReadCartridge(0x0286);
			sequence[1] = console.DebugReadCartridge(0x0286);
			sequence[2] = console.DebugReadCartridge(0x0287);
			sequence[3] = console.DebugReadCartridge(0x0284);
			sequence[4] = console.DebugReadCartridge(0x0287);
			sequence[5] = console.DebugReadCartridge(0x0286);
			return sequence;
		};

		auto runA = runSequence();
		auto runB = runSequence();

		EXPECT_EQ(runA, runB);
		EXPECT_EQ(runA[0], 1u);
		EXPECT_EQ(runA[1], 0u);
		EXPECT_EQ(runA[2], 1u);
		EXPECT_EQ(runA[3], 0xFFu);
		EXPECT_EQ(runA[4], 0u);
		EXPECT_EQ(runA[5], 0u);
	}
}
