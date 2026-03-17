#include "pch.h"
#include "Genesis/GenesisM68kBoundaryScaffold.h"

namespace {
	TEST(GenesisPsgMixedAudioTests, PsgWritesThroughVdpPortUpdateRegisterState) {
		GenesisM68kBoundaryScaffold scaffold;
		scaffold.Startup();

		scaffold.GetBus().WriteByte(0xC00011, 0x90);
		scaffold.GetBus().WriteByte(0xC00011, 0x05);

		EXPECT_EQ(scaffold.GetBus().GetPsgWriteCount(), 2u);
		EXPECT_NE(scaffold.GetBus().GetPsgRegister(1), 0u);
	}

	TEST(GenesisPsgMixedAudioTests, PsgTimingProgressesWithStepCycles) {
		GenesisM68kBoundaryScaffold scaffold;
		scaffold.Startup();
		scaffold.GetBus().WriteByte(0xC00011, 0x90);
		scaffold.GetBus().WriteByte(0xC00011, 0x08);

		scaffold.StepFrameScaffold(127);
		EXPECT_EQ(scaffold.GetBus().GetPsgSampleCount(), 0u);
		scaffold.StepFrameScaffold(1);
		EXPECT_EQ(scaffold.GetBus().GetPsgSampleCount(), 1u);
	}

	TEST(GenesisPsgMixedAudioTests, MixedOutputDigestIsDeterministicAcrossRuns) {
		GenesisM68kBoundaryScaffold scaffoldA;
		scaffoldA.Startup();
		scaffoldA.GetBus().WriteByte(0xA04000, 0x22);
		scaffoldA.GetBus().WriteByte(0xA04001, 0x12);
		scaffoldA.GetBus().WriteByte(0xC00011, 0x90);
		scaffoldA.GetBus().WriteByte(0xC00011, 0x04);
		scaffoldA.StepFrameScaffold(144u * 8u);

		GenesisM68kBoundaryScaffold scaffoldB;
		scaffoldB.Startup();
		scaffoldB.GetBus().WriteByte(0xA04000, 0x22);
		scaffoldB.GetBus().WriteByte(0xA04001, 0x12);
		scaffoldB.GetBus().WriteByte(0xC00011, 0x90);
		scaffoldB.GetBus().WriteByte(0xC00011, 0x04);
		scaffoldB.StepFrameScaffold(144u * 8u);

		EXPECT_EQ(scaffoldA.GetBus().GetMixedSampleCount(), scaffoldB.GetBus().GetMixedSampleCount());
		EXPECT_EQ(scaffoldA.GetBus().GetMixedLastSample(), scaffoldB.GetBus().GetMixedLastSample());
		EXPECT_EQ(scaffoldA.GetBus().GetMixedDigest(), scaffoldB.GetBus().GetMixedDigest());
	}
}
