#include "rgb_driver.h"
#include "main.h"

void rgb_driver_thread_entry(ULONG thread_input)
{
  UNUSED(thread_input);
  while (1)
  {
      HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
      tx_thread_sleep(50);  // Sleep for 50 ticks
  }
}