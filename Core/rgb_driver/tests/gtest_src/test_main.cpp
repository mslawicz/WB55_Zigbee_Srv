
#include "gtest/gtest.h"

extern "C" {
#include "mock_stm32wbxx_hal.h"
#include "mock_tx_api.h"
#include "rgb_driver.h"
}

TEST(convert_xy_to_RGB, color_white) {
    RGB rgb = convert_xy_to_RGB(XY{21167, 21561});
    EXPECT_GT(rgb.R, 250);
    EXPECT_GT(rgb.G, 250);
    EXPECT_GT(rgb.B, 250);
}

TEST(convert_xy_to_RGB, color_red) {
    RGB rgb = convert_xy_to_RGB(XY{45808, 19594});
    EXPECT_GT(rgb.R, 250);
    EXPECT_LT(rgb.G, 20);
    EXPECT_LT(rgb.B, 20);
}

TEST(convert_xy_to_RGB, color_blue) {
    RGB rgb = convert_xy_to_RGB(XY{8912, 2621});
    EXPECT_LT(rgb.R, 20);
    EXPECT_LT(rgb.G, 20);
    EXPECT_GT(rgb.B, 250);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
