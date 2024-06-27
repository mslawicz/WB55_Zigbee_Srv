
#include "gtest/gtest.h"

extern "C" {
#include "mock_stm32wbxx_hal.h"
#include "mock_tx_api.h"
#include "myTest.h"
#include "rgb_driver.h"
}

TEST(MyCodeTest, TestMyFunction) {
    // Call the function to be tested
    EXPECT_EQ(mySqr(2), 4);
    // Assert expected behavior
    // Here you would check if the mocked HAL function was called as expected
    // This might involve setting flags or using a mock framework to verify calls
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
