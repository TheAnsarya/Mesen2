#include "pch.h"
// MetadataRecordingBench.cpp
// Benchmarks for CDL and Pansy metadata recording overhead
// Focus: enabled vs disabled, and combined pipeline

#include "benchmark/benchmark.h"
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <random>

// Dummy CDL recorder
class DummyCdlRecorder {
public:
	bool enabled = false;
	void SetCode(uint32_t addr) {
		if (enabled) {
			volatile uint8_t sink = addr & 0xff;
		}
	}
	void SetData(uint32_t addr) {
		if (enabled) {
			volatile uint8_t sink = addr & 0xff;
		}
	}
};

// Dummy Pansy writer
class DummyPansyWriter {
public:
	bool enabled = false;
	void Export(uint32_t addr) {
		if (enabled) {
			volatile uint8_t sink = addr & 0xff;
		}
	}
};

static void BM_CDL_SetCode_Enabled(benchmark::State& state) {
	DummyCdlRecorder cdl;
	cdl.enabled = true;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			cdl.SetCode(i);
		}
	}
}
BENCHMARK(BM_CDL_SetCode_Enabled);

static void BM_CDL_SetCode_Disabled(benchmark::State& state) {
	DummyCdlRecorder cdl;
	cdl.enabled = false;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			cdl.SetCode(i);
		}
	}
}
BENCHMARK(BM_CDL_SetCode_Disabled);

static void BM_Pansy_Export_Enabled(benchmark::State& state) {
	DummyPansyWriter pansy;
	pansy.enabled = true;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			pansy.Export(i);
		}
	}
}
BENCHMARK(BM_Pansy_Export_Enabled);

static void BM_Pansy_Export_Disabled(benchmark::State& state) {
	DummyPansyWriter pansy;
	pansy.enabled = false;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			pansy.Export(i);
		}
	}
}
BENCHMARK(BM_Pansy_Export_Disabled);

static void BM_Metadata_Combined_Enabled(benchmark::State& state) {
	DummyCdlRecorder cdl;
	DummyPansyWriter pansy;
	cdl.enabled = true;
	pansy.enabled = true;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			cdl.SetCode(i);
			cdl.SetData(i);
			pansy.Export(i);
		}
	}
}
BENCHMARK(BM_Metadata_Combined_Enabled);

static void BM_Metadata_Combined_Disabled(benchmark::State& state) {
	DummyCdlRecorder cdl;
	DummyPansyWriter pansy;
	cdl.enabled = false;
	pansy.enabled = false;
	for (auto _ : state) {
		for (uint32_t i = 0; i < 1000; i++) {
			cdl.SetCode(i);
			cdl.SetData(i);
			pansy.Export(i);
		}
	}
}
BENCHMARK(BM_Metadata_Combined_Disabled);
