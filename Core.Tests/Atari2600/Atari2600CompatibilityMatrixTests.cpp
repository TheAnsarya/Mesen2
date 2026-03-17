#include "pch.h"
#include "Shared/Emulator.h"
#include "Atari2600/Atari2600Console.h"
#include "Atari2600/Atari2600SmokeHarness.h"

namespace {
	vector<Atari2600BaselineRomCase> BuildCompatibilityCorpus() {
		vector<Atari2600BaselineRomCase> corpus;
		corpus.push_back({"compat-target-f8.a26", vector<uint8_t>(8192, 0xEA)});
		corpus.push_back({"compat-target-f6.a26", vector<uint8_t>(16384, 0xEA)});
		corpus.push_back({"compat-target-3f.a26", vector<uint8_t>(8192, 0xEA)});
		return corpus;
	}

	TEST(Atari2600CompatibilityMatrixTests, CompatibilityCorpusProducesDeterministicMatrixDigest) {
		Emulator emu;
		Atari2600Console console(&emu);
		vector<Atari2600BaselineRomCase> corpus = BuildCompatibilityCorpus();

		Atari2600CompatibilityMatrixResult runA = Atari2600SmokeHarness::RunCompatibilityMatrix(console, corpus);
		Atari2600CompatibilityMatrixResult runB = Atari2600SmokeHarness::RunCompatibilityMatrix(console, corpus);

		ASSERT_EQ(runA.Entries.size(), corpus.size());
		EXPECT_EQ(runA.PassCount, (int)corpus.size());
		EXPECT_EQ(runA.FailCount, 0);
		EXPECT_FALSE(runA.Digest.empty());
		EXPECT_EQ(runA.Digest, runB.Digest);

		bool hasPerTitleResult = std::any_of(runA.OutputLines.begin(), runA.OutputLines.end(), [](const string& line) {
			return line.starts_with("COMPAT_RESULT ");
		});
		bool hasSummary = std::any_of(runA.OutputLines.begin(), runA.OutputLines.end(), [](const string& line) {
			return line.starts_with("COMPAT_MATRIX_SUMMARY ");
		});

		EXPECT_TRUE(hasPerTitleResult);
		EXPECT_TRUE(hasSummary);
	}

	TEST(Atari2600CompatibilityMatrixTests, EmptyRomCaseFailsWithDeterministicCheckpoint) {
		Emulator emu;
		Atari2600Console console(&emu);

		vector<Atari2600BaselineRomCase> corpus;
		corpus.push_back({"compat-empty.a26", {}});

		Atari2600CompatibilityMatrixResult result = Atari2600SmokeHarness::RunCompatibilityMatrix(console, corpus);
		ASSERT_EQ(result.Entries.size(), 1u);
		EXPECT_EQ(result.PassCount, 0);
		EXPECT_EQ(result.FailCount, 1);
		EXPECT_FALSE(result.Digest.empty());
		EXPECT_FALSE(result.Entries[0].Checkpoints.empty());
		EXPECT_FALSE(result.Entries[0].Pass);
	}
}
