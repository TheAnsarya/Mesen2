using BenchmarkDotNet.Configs;
using BenchmarkDotNet.Running;

namespace Nexen.Benchmarks;

public class Program {
	public static void Main(string[] args) {
		// Run with category filter to avoid baseline conflicts
		// DisableOptimizationsValidator because UI project has special Release config
		var config = DefaultConfig.Instance
			.WithOptions(ConfigOptions.JoinSummary)
			.WithOptions(ConfigOptions.DisableOptimizationsValidator);

		// Run all benchmarks from this assembly
		BenchmarkSwitcher.FromAssembly(typeof(Program).Assembly).Run(args, config);
	}
}
