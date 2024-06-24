#include "gtest/gtest.h"
//#include "../stm32wbxx_hal.h"

// TEST(HALTest, Initialization) {
//     HAL_StatusTypeDef status = HAL_Init();
//     ASSERT_EQ(status, HAL_OK);
// }

TEST(intTest, chekValue) {
    int x = 1;
    ASSERT_EQ(x-1, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}