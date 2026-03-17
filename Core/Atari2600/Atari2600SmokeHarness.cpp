#include "pch.h"
#include "Atari2600/Atari2600SmokeHarness.h"
#include "Atari2600/Atari2600Console.h"
#include "Utilities/VirtualFile.h"

namespace {
	string ToHex(uint64_t value) {
		return std::format("{:016x}", value);
	}

	string BuildDigest(const vector<Atari2600HarnessCheckpoint>& checkpoints) {
		uint64_t hash = 1469598103934665603ull;
		for (const Atari2600HarnessCheckpoint& cp : checkpoints) {
			string line = cp.Id + ":" + (cp.Pass ? "PASS" : "FAIL") + ":" + cp.Context;
			for (uint8_t ch : line) {
				hash ^= ch;
				hash *= 1099511628211ull;
			}
		}
		return ToHex(hash);
	}
}

Atari2600HarnessResult Atari2600SmokeHarness::RunBaseline(Atari2600Console& console) {
	Atari2600HarnessResult result = {};

	auto addCheckpoint = [&](string id, bool pass, string context) {
		Atari2600HarnessCheckpoint cp = {};
		cp.Id = std::move(id);
		cp.Pass = pass;
		cp.Context = std::move(context);
		if (cp.Pass) {
			result.PassCount++;
		} else {
			result.FailCount++;
		}
		result.OutputLines.push_back(std::format("CHECKPOINT {} {} {}", cp.Id, cp.Pass ? "PASS" : "FAIL", cp.Context));
		result.Checkpoints.push_back(std::move(cp));
	};

	console.Reset();
	console.StepCpuCycles(Atari2600Console::CpuCyclesPerFrame);
	Atari2600TiaState frameBoundaryState = console.GetTiaState();
	addCheckpoint("TIA-CP-01", frameBoundaryState.FrameCount == 1 && frameBoundaryState.Scanline == 0,
		std::format("frame={} scanline={}", frameBoundaryState.FrameCount, frameBoundaryState.Scanline));

	console.Reset();
	console.StepCpuCycles(9);
	uint32_t originalScanline = console.GetTiaState().Scanline;
	console.RequestWsync();
	console.StepCpuCycles(1);
	Atari2600TiaState wsyncState = console.GetTiaState();
	addCheckpoint("TIA-CP-02", wsyncState.Scanline == originalScanline + 1,
		std::format("scanlineBefore={} scanlineAfter={}", originalScanline, wsyncState.Scanline));

	console.Reset();
	console.StepCpuCycles(42);
	Atari2600TiaState visibilityState = console.GetTiaState();
	bool inExpectedBucket = visibilityState.ColorClock >= 120 && visibilityState.ColorClock <= 132;
	addCheckpoint("TIA-CP-03", inExpectedBucket,
		std::format("colorClock={}", visibilityState.ColorClock));

	string digestA = BuildDigest(result.Checkpoints);
	string digestB = BuildDigest(result.Checkpoints);
	addCheckpoint("TIA-CP-04", digestA == digestB, std::format("digestA={} digestB={}", digestA, digestB));

	result.Digest = BuildDigest(result.Checkpoints);
	result.OutputLines.push_back(std::format("HARNESS_SUMMARY PASS={} FAIL={} DIGEST={}", result.PassCount, result.FailCount, result.Digest));
	return result;
}

Atari2600TimingSpikeResult Atari2600SmokeHarness::RunTimingSpike(Atari2600Console& console, uint32_t iterations) {
	Atari2600TimingSpikeResult result = {};
	result.ScanlineDeltas.reserve(iterations);

	console.Reset();
	result.InitialScanline = console.GetTiaState().Scanline;

	for (uint32_t i = 0; i < iterations; i++) {
		uint32_t beforeScanline = console.GetTiaState().Scanline;
		console.StepCpuCycles(76);
		uint32_t afterScanline = console.GetTiaState().Scanline;
		uint32_t delta = afterScanline - beforeScanline;
		result.ScanlineDeltas.push_back(delta);
		result.OutputLines.push_back(std::format("TIMING_SPIKE STEP={} BEFORE={} AFTER={} DELTA={}", i, beforeScanline, afterScanline, delta));
	}

	Atari2600TiaState beforeWsync = console.GetTiaState();
	console.StepCpuCycles(9);
	uint32_t scanlinePreWsync = console.GetTiaState().Scanline;
	console.RequestWsync();
	console.StepCpuCycles(1);
	Atari2600TiaState afterWsync = console.GetTiaState();
	result.OutputLines.push_back(std::format(
		"TIMING_SPIKE WSYNC SCANLINE_BEFORE={} SCANLINE_AFTER={} COLORCLOCK_BEFORE={} COLORCLOCK_AFTER={}",
		scanlinePreWsync,
		afterWsync.Scanline,
		beforeWsync.ColorClock,
		afterWsync.ColorClock));

	result.Stable = std::all_of(result.ScanlineDeltas.begin(), result.ScanlineDeltas.end(), [](uint32_t delta) {
		return delta == 1;
	});

	result.FinalScanline = afterWsync.Scanline;
	result.FinalColorClock = afterWsync.ColorClock;

	string digestInput = std::format(
		"stable={} init={} final={} color={} wsyncDelta={}",
		result.Stable ? 1 : 0,
		result.InitialScanline,
		result.FinalScanline,
		result.FinalColorClock,
		afterWsync.Scanline - scanlinePreWsync);

	for (uint32_t delta : result.ScanlineDeltas) {
		digestInput += std::format(":{}", delta);
	}

	uint64_t hash = 1469598103934665603ull;
	for (uint8_t ch : digestInput) {
		hash ^= ch;
		hash *= 1099511628211ull;
	}

	result.Digest = ToHex(hash);
	result.OutputLines.push_back(std::format("TIMING_SPIKE SUMMARY STABLE={} DIGEST={}", result.Stable ? "true" : "false", result.Digest));
	return result;
}

Atari2600BaselineRomSetResult Atari2600SmokeHarness::RunBaselineRomSet(Atari2600Console& console, const vector<Atari2600BaselineRomCase>& romSet) {
	Atari2600BaselineRomSetResult result = {};
	result.Entries.reserve(romSet.size());

	for (const Atari2600BaselineRomCase& romCase : romSet) {
		Atari2600BaselineRomEntry entry = {};
		entry.Name = romCase.Name;

		if (romCase.RomData.empty()) {
			entry.Pass = false;
			entry.Result.FailCount = 1;
			entry.Result.OutputLines.push_back("HARNESS_LOAD FAIL empty_rom_data");
			result.OutputLines.push_back(std::format("ROM_RESULT {} FAIL reason=empty_rom_data", entry.Name));
			result.FailCount++;
			result.Entries.push_back(std::move(entry));
			continue;
		}

		VirtualFile romFile(romCase.RomData.data(), romCase.RomData.size(), romCase.Name);
		LoadRomResult loadResult = console.LoadRom(romFile);
		if (loadResult != LoadRomResult::Success) {
			entry.Pass = false;
			entry.Result.FailCount = 1;
			entry.Result.OutputLines.push_back(std::format("HARNESS_LOAD FAIL load_result={}", (int)loadResult));
			result.OutputLines.push_back(std::format("ROM_RESULT {} FAIL reason=load_result_{}", entry.Name, (int)loadResult));
			result.FailCount++;
			result.Entries.push_back(std::move(entry));
			continue;
		}

		entry.Result = RunBaseline(console);
		entry.Pass = entry.Result.FailCount == 0;
		if (entry.Pass) {
			result.PassCount++;
		} else {
			result.FailCount++;
		}

		result.OutputLines.push_back(std::format(
			"ROM_RESULT {} {} PASS={} FAIL={} DIGEST={}",
			entry.Name,
			entry.Pass ? "PASS" : "FAIL",
			entry.Result.PassCount,
			entry.Result.FailCount,
			entry.Result.Digest));

		result.Entries.push_back(std::move(entry));
	}

	uint64_t hash = 1469598103934665603ull;
	for (const Atari2600BaselineRomEntry& entry : result.Entries) {
		string line = std::format("{}:{}:{}:{}", entry.Name, entry.Pass ? "PASS" : "FAIL", entry.Result.PassCount, entry.Result.Digest);
		for (uint8_t ch : line) {
			hash ^= ch;
			hash *= 1099511628211ull;
		}
	}

	result.Digest = ToHex(hash);
	result.OutputLines.push_back(std::format("ROM_SET_SUMMARY PASS={} FAIL={} DIGEST={}", result.PassCount, result.FailCount, result.Digest));
	return result;
}
