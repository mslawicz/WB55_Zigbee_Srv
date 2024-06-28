
#include "gtest/gtest.h"

extern "C" {
#include "mock_stm32wbxx_hal.h"
#include "mock_tx_api.h"
#include "rgb_driver.h"
}

// tests of xy to RGB conversion

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

// tests of color temperature to xy conversion

TEST(color_temperature_to_xy, temperature_high) {
    XY xy = color_temperature_to_xy(100);
    EXPECT_GT(xy.x, 17800);
    EXPECT_LT(xy.x, 17850);
    EXPECT_GT(xy.y, 18900);
    EXPECT_LT(xy.y, 18950);    
}

TEST(color_temperature_to_xy, temperature_neutral) {
    XY xy = color_temperature_to_xy(350);
    EXPECT_GT(xy.x, 33450);
    EXPECT_LT(xy.x, 33500);
    EXPECT_GT(xy.y, 25400);
    EXPECT_LT(xy.y, 25470);    
}

TEST(color_temperature_to_xy, temperature_low) {
    XY xy = color_temperature_to_xy(500);
    EXPECT_GT(xy.x, 39850);
    EXPECT_LT(xy.x, 39900);
    EXPECT_GT(xy.y, 24650);
    EXPECT_LT(xy.y, 24700);    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
