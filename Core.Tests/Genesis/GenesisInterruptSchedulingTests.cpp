#include "pch.h"
#include "Genesis/GenesisM68kBoundaryScaffold.h"

namespace {
	TEST(GenesisInterruptSchedulingTests, HorizontalInterruptFiresAtConfiguredInterval) {
		GenesisM68kBoundaryScaffold scaffold;
		scaffold.Startup();
		scaffold.ConfigureInterruptSchedule(true, 8, false);
		scaffold.ClearTimingEvents();

		scaffold.StepFrameScaffold(488u * 24u);

		EXPECT_EQ(scaffold.GetHorizontalInterruptCount(), 3u);
		EXPECT_EQ(scaffold.GetVerticalInterruptCount(), 0u);
		EXPECT_EQ(scaffold.GetTimingEvents().size(), 3u);
		EXPECT_TRUE(scaffold.GetTimingEvents().at(0).starts_with("HINT "));
	}

	TEST(GenesisInterruptSchedulingTests, VerticalInterruptFiresOnFrameRollover) {
		GenesisM68kBoundaryScaffold scaffold;
		scaffold.Startup();
		scaffold.ConfigureInterruptSchedule(false, 16, true);
		scaffold.ClearTimingEvents();

		scaffold.StepFrameScaffold(488u * 262u);

		EXPECT_EQ(scaffold.GetHorizontalInterruptCount(), 0u);
		EXPECT_EQ(scaffold.GetVerticalInterruptCount(), 1u);
		EXPECT_EQ(scaffold.GetTimingFrame(), 1u);
		EXPECT_EQ(scaffold.GetTimingScanline(), 0u);
		EXPECT_EQ(scaffold.GetCpu().GetInterruptLevel(), 6u);
	}

	TEST(GenesisInterruptSchedulingTests, EventOrderingIsDeterministicWithBothInterruptsEnabled) {
		GenesisM68kBoundaryScaffold scaffold;
		scaffold.Startup();
		scaffold.ConfigureInterruptSchedule(true, 1, true);
		scaffold.ClearTimingEvents();

		scaffold.StepFrameScaffold(488u * 262u);
		const vector<string>& events = scaffold.GetTimingEvents();

		ASSERT_GE(events.size(), 2u);
		EXPECT_TRUE(events.at(events.size() - 2).starts_with("HINT "));
		EXPECT_TRUE(events.at(events.size() - 1).starts_with("VINT "));
	}
}
