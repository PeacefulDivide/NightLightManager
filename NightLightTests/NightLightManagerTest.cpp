#include "pch.h"
#include "NightLightManager.h"

TEST(GammaLogic, IsMonotonic) {
	WORD ramp[3][256];
	NightLightManager::CalculateRamp(true, 0.8f, ramp);

	for (int i = 1; i < 256; i++) {
		// Increasing values
		EXPECT_GE(ramp[0][i], ramp[0][i - 1]);
		EXPECT_GE(ramp[1][i], ramp[1][i - 1]);
		EXPECT_GE(ramp[2][i], ramp[2][i - 1]);
	}
}