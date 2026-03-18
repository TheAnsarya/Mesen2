#include "pch.h"
#include "Shared/Emulator.h"
#include "Atari2600/Atari2600Console.h"
#include "Atari2600/Atari2600SmokeHarness.h"
#include "Utilities/VirtualFile.h"

namespace {
	vector<Atari2600BaselineRomCase> CreateBaselineRomSet() {
		vector<Atari2600BaselineRomCase> romSet;

		Atari2600BaselineRomCase nopRom = {};
		nopRom.Name = "baseline-nop-fill.a26";
		nopRom.RomData.assign(4096, 0xEA);
		romSet.push_back(std::move(nopRom));

		Atari2600BaselineRomCase zeroRom = {};
		zeroRom.Name = "baseline-zero-fill.a26";
		zeroRom.RomData.assign(4096, 0x00);
		romSet.push_back(std::move(zeroRom));

		Atari2600BaselineRomCase rampRom = {};
		rampRom.Name = "baseline-ramp-pattern.a26";
		rampRom.RomData.resize(4096);
		for (size_t i = 0; i < rampRom.RomData.size(); i++) {
			rampRom.RomData[i] = (uint8_t)(i & 0xFF);
		}
		romSet.push_back(std::move(rampRom));

		return romSet;
	}

	TEST(Atari2600TimingSpikeHarnessTests, RiotSkeletonTracksCpuCycleProgress) {
		Emulator emu;
		Atari2600Console console(&emu);

		console.Reset();
		console.StepCpuCycles(4);
		Atari2600RiotState riotState = console.GetRiotState();

		EXPECT_EQ(riotState.CpuCycles, 4u);
		EXPECT_TRUE(riotState.TimerUnderflow);
	}

	TEST(Atari2600TimingSpikeHarnessTests, SmokeRomFrameStepCompletesWithoutCrash) {
		Emulator emu;
		Atari2600Console console(&emu);

		vector<uint8_t> rom(4096, 0xEA);
		VirtualFile smokeRom(rom.data(), rom.size(), "smoke.a26");

		EXPECT_EQ(console.LoadRom(smokeRom), LoadRomResult::Success);
		console.RunFrame();

		Atari2600FrameStepSummary summary = console.GetLastFrameSummary();
		EXPECT_EQ(summary.FrameCount, 1u);
		EXPECT_EQ(summary.CpuCyclesThisFrame, Atari2600Console::CpuCyclesPerFrame);
	}

	TEST(Atari2600TimingSpikeHarnessTests, BaselineHarnessReturnsPassingCheckpointSet) {
		Emulator emu;
		Atari2600Console console(&emu);

		Atari2600HarnessResult result = Atari2600SmokeHarness::RunBaseline(console);

		EXPECT_GT(result.PassCount, 0);
		EXPECT_EQ(result.FailCount, 0);
		EXPECT_FALSE(result.Digest.empty());
	}

	TEST(Atari2600TimingSpikeHarnessTests, TimingSpikeHarnessHasStableScanlineDeltas) {
		Emulator emu;
		Atari2600Console console(&emu);

		Atari2600TimingSpikeResult result = Atari2600SmokeHarness::RunTimingSpike(console, 12);

		EXPECT_TRUE(result.Stable);
		EXPECT_EQ(result.ScanlineDeltas.size(), 12u);
		for (uint32_t delta : result.ScanlineDeltas) {
			EXPECT_EQ(delta, 1u);
		}
	}

	TEST(Atari2600TimingSpikeHarnessTests, TimingSpikeHarnessDigestIsDeterministic) {
		Emulator emu;
		Atari2600Console console(&emu);

		Atari2600TimingSpikeResult runA = Atari2600SmokeHarness::RunTimingSpike(console, 10);
		Atari2600TimingSpikeResult runB = Atari2600SmokeHarness::RunTimingSpike(console, 10);

		EXPECT_FALSE(runA.Digest.empty());
		EXPECT_EQ(runA.Digest, runB.Digest);
	}

	TEST(Atari2600TimingSpikeHarnessTests, BaselineRomSetProducesPerRomPassFailResults) {
		Emulator emu;
		Atari2600Console console(&emu);
		vector<Atari2600BaselineRomCase> romSet = CreateBaselineRomSet();

		Atari2600BaselineRomSetResult result = Atari2600SmokeHarness::RunBaselineRomSet(console, romSet);

		EXPECT_EQ(result.Entries.size(), romSet.size());
		EXPECT_EQ(result.PassCount, 3);
		EXPECT_EQ(result.FailCount, 0);
		EXPECT_FALSE(result.Digest.empty());
		for (const Atari2600BaselineRomEntry& entry : result.Entries) {
			EXPECT_TRUE(entry.Pass);
			EXPECT_EQ(entry.Result.FailCount, 0);
		}

		bool hasRomResultLine = std::any_of(result.OutputLines.begin(), result.OutputLines.end(), [](const string& line) {
			return line.starts_with("ROM_RESULT ");
		});
		bool hasRomSetSummaryLine = std::any_of(result.OutputLines.begin(), result.OutputLines.end(), [](const string& line) {
			return line.starts_with("ROM_SET_SUMMARY ");
		});

		EXPECT_TRUE(hasRomResultLine);
		EXPECT_TRUE(hasRomSetSummaryLine);
	}

	TEST(Atari2600TimingSpikeHarnessTests, BaselineRomSetDigestIsDeterministic) {
		Emulator emu;
		Atari2600Console console(&emu);
		vector<Atari2600BaselineRomCase> romSet = CreateBaselineRomSet();

		Atari2600BaselineRomSetResult runA = Atari2600SmokeHarness::RunBaselineRomSet(console, romSet);
		Atari2600BaselineRomSetResult runB = Atari2600SmokeHarness::RunBaselineRomSet(console, romSet);

		EXPECT_FALSE(runA.Digest.empty());
		EXPECT_EQ(runA.Digest, runB.Digest);
	}

	TEST(Atari2600TimingSpikeHarnessTests, BaselineRomSetFailureIncludesTriageContext) {
		Emulator emu;
		Atari2600Console console(&emu);
		vector<Atari2600BaselineRomCase> romSet;
		romSet.push_back({"broken-empty.a26", {}});

		Atari2600BaselineRomSetResult result = Atari2600SmokeHarness::RunBaselineRomSet(console, romSet);
		EXPECT_EQ(result.PassCount, 0);
		EXPECT_EQ(result.FailCount, 1);

		bool hasFailContext = std::any_of(result.OutputLines.begin(), result.OutputLines.end(), [](const string& line) {
			return line.starts_with("ROM_FAIL_CONTEXT ");
		});
		EXPECT_TRUE(hasFailContext);
	}
}
