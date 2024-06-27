
#include "gtest/gtest.h"

extern "C" {
#include "mock_stm32wbxx_hal.h"
#include "mock_tx_api.h"
#include "rgb_driver.h"
}

TEST(convert_xy_to_RGB, not_zero) {
    // Call the function to be tested
    RGB rgb = convert_xy_to_RGB(XY{10000, 15000});
    EXPECT_NE(rgb.B, 0);
    // Assert expected behavior
    // Here you would check if the mocked HAL function was called as expected
    // This might involve setting flags or using a mock framework to verify calls
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
