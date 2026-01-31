#include "pch.h"
#include <array>
#include "NES/NesTypes.h"

// =============================================================================
// NES CPU State Benchmarks
// =============================================================================

// Benchmark flag manipulation - common operation in CPU emulation
static void BM_NesCpu_FlagManipulation(benchmark::State& state) {
	NesCpuState cpuState = {};
	cpuState.PS = 0x24;  // Initial: I flag + Reserved
	
	for (auto _ : state) {
		// Simulate typical flag updates
		cpuState.PS |= PSFlags::Carry;
		cpuState.PS &= ~PSFlags::Zero;
		cpuState.PS |= PSFlags::Negative;
		cpuState.PS &= ~PSFlags::Overflow;
		benchmark::DoNotOptimize(cpuState.PS);
	}
	state.SetItemsProcessed(state.iterations() * 4);
}
BENCHMARK(BM_NesCpu_FlagManipulation);

// Benchmark Zero and Negative flag calculation (very frequent operation)
static void BM_NesCpu_SetZeroNegativeFlags(benchmark::State& state) {
	NesCpuState cpuState = {};
	uint8_t value = 0;
	
	for (auto _ : state) {
		// Clear Z and N flags
		cpuState.PS &= ~(PSFlags::Zero | PSFlags::Negative);
		// Set based on value
		if (value == 0) cpuState.PS |= PSFlags::Zero;
		if (value & 0x80) cpuState.PS |= PSFlags::Negative;
		benchmark::DoNotOptimize(cpuState.PS);
		value++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_SetZeroNegativeFlags);

// Benchmark branchless Zero/Negative flag setting (comparison)
static void BM_NesCpu_SetZeroNegativeFlags_Branchless(benchmark::State& state) {
	NesCpuState cpuState = {};
	uint8_t value = 0;
	
	for (auto _ : state) {
		cpuState.PS &= ~(PSFlags::Zero | PSFlags::Negative);
		cpuState.PS |= (value == 0) ? PSFlags::Zero : 0;
		cpuState.PS |= (value >> 7) << 7;  // Move bit 7 to Negative flag position
		benchmark::DoNotOptimize(cpuState.PS);
		value++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_SetZeroNegativeFlags_Branchless);

// Benchmark stack operations (push/pop patterns)
static void BM_NesCpu_StackPushPop(benchmark::State& state) {
	NesCpuState cpuState = {};
	cpuState.SP = 0xFF;
	std::array<uint8_t, 256> stack{};
	
	for (auto _ : state) {
		// Push pattern
		stack[0x100 + cpuState.SP--] = cpuState.A;
		stack[0x100 + cpuState.SP--] = cpuState.X;
		// Pop pattern
		cpuState.X = stack[0x100 + ++cpuState.SP];
		cpuState.A = stack[0x100 + ++cpuState.SP];
		benchmark::DoNotOptimize(cpuState.SP);
		benchmark::DoNotOptimize(cpuState.A);
	}
	state.SetItemsProcessed(state.iterations() * 4);  // 4 stack operations
}
BENCHMARK(BM_NesCpu_StackPushPop);

// Benchmark address mode calculations - Zero Page
static void BM_NesCpu_AddrMode_ZeroPage(benchmark::State& state) {
	uint8_t operand = 0x42;
	
	for (auto _ : state) {
		uint16_t addr = operand;
		benchmark::DoNotOptimize(addr);
		operand++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_AddrMode_ZeroPage);

// Benchmark address mode calculations - Zero Page X
static void BM_NesCpu_AddrMode_ZeroPageX(benchmark::State& state) {
	uint8_t operand = 0x42;
	uint8_t X = 0x10;
	
	for (auto _ : state) {
		uint16_t addr = static_cast<uint8_t>(operand + X);  // Wraps in zero page
		benchmark::DoNotOptimize(addr);
		operand++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_AddrMode_ZeroPageX);

// Benchmark address mode calculations - Absolute
static void BM_NesCpu_AddrMode_Absolute(benchmark::State& state) {
	uint8_t lowByte = 0x42;
	uint8_t highByte = 0x80;
	
	for (auto _ : state) {
		uint16_t addr = lowByte | (static_cast<uint16_t>(highByte) << 8);
		benchmark::DoNotOptimize(addr);
		lowByte++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_AddrMode_Absolute);

// Benchmark address mode calculations - Absolute X with page crossing check
static void BM_NesCpu_AddrMode_AbsoluteX_PageCross(benchmark::State& state) {
	uint16_t baseAddr = 0x80F0;
	uint8_t X = 0x20;
	
	for (auto _ : state) {
		uint16_t addr = baseAddr + X;
		bool pageCrossed = ((baseAddr & 0xFF00) != (addr & 0xFF00));
		benchmark::DoNotOptimize(addr);
		benchmark::DoNotOptimize(pageCrossed);
		baseAddr++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_AddrMode_AbsoluteX_PageCross);

// Benchmark indirect indexed (Y) - most complex common mode
static void BM_NesCpu_AddrMode_IndirectY(benchmark::State& state) {
	std::array<uint8_t, 256> zeroPage{};
	zeroPage[0x42] = 0x00;
	zeroPage[0x43] = 0x80;
	uint8_t zpAddr = 0x42;
	uint8_t Y = 0x10;
	
	for (auto _ : state) {
		uint16_t baseAddr = zeroPage[zpAddr] | (static_cast<uint16_t>(zeroPage[zpAddr + 1]) << 8);
		uint16_t addr = baseAddr + Y;
		bool pageCrossed = ((baseAddr & 0xFF00) != (addr & 0xFF00));
		benchmark::DoNotOptimize(addr);
		benchmark::DoNotOptimize(pageCrossed);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_AddrMode_IndirectY);

// Benchmark ADC instruction pattern (8-bit with carry)
static void BM_NesCpu_Instruction_ADC(benchmark::State& state) {
	NesCpuState cpuState = {};
	cpuState.A = 0x40;
	cpuState.PS = 0;
	uint8_t operand = 0x30;
	
	for (auto _ : state) {
		uint16_t result = cpuState.A + operand + (cpuState.PS & PSFlags::Carry);
		
		// Set flags
		cpuState.PS &= ~(PSFlags::Carry | PSFlags::Zero | PSFlags::Negative | PSFlags::Overflow);
		if (result > 0xFF) cpuState.PS |= PSFlags::Carry;
		if ((result & 0xFF) == 0) cpuState.PS |= PSFlags::Zero;
		if (result & 0x80) cpuState.PS |= PSFlags::Negative;
		// Overflow: sign of result differs from sign of both inputs
		if (~(cpuState.A ^ operand) & (cpuState.A ^ result) & 0x80) {
			cpuState.PS |= PSFlags::Overflow;
		}
		
		cpuState.A = static_cast<uint8_t>(result);
		benchmark::DoNotOptimize(cpuState.A);
		benchmark::DoNotOptimize(cpuState.PS);
		operand++;
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_Instruction_ADC);

// Benchmark branch instruction pattern
static void BM_NesCpu_Instruction_Branch(benchmark::State& state) {
	NesCpuState cpuState = {};
	cpuState.PC = 0x8000;
	cpuState.PS = PSFlags::Zero;  // BEQ will be taken
	int8_t offset = 10;
	
	for (auto _ : state) {
		bool taken = (cpuState.PS & PSFlags::Zero) != 0;
		if (taken) {
			uint16_t oldPage = cpuState.PC & 0xFF00;
			cpuState.PC += offset;
			bool pageCrossed = (cpuState.PC & 0xFF00) != oldPage;
			benchmark::DoNotOptimize(pageCrossed);
		}
		benchmark::DoNotOptimize(cpuState.PC);
		cpuState.PC++;  // Reset for next iteration
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_Instruction_Branch);

// Benchmark typical read-modify-write pattern (INC, DEC, shifts)
static void BM_NesCpu_Instruction_RMW_Pattern(benchmark::State& state) {
	uint8_t memory = 0x42;
	NesCpuState cpuState = {};
	
	for (auto _ : state) {
		// Read
		uint8_t value = memory;
		// Modify
		value++;
		// Write
		memory = value;
		// Set flags
		cpuState.PS &= ~(PSFlags::Zero | PSFlags::Negative);
		if (value == 0) cpuState.PS |= PSFlags::Zero;
		if (value & 0x80) cpuState.PS |= PSFlags::Negative;
		benchmark::DoNotOptimize(memory);
		benchmark::DoNotOptimize(cpuState.PS);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_NesCpu_Instruction_RMW_Pattern);

