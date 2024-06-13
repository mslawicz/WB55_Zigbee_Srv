#include "rgb_driver.h"
#include "main.h"

TX_EVENT_FLAGS_GROUP rgb_driver_flags;

void check_flags(ULONG flags);

void rgb_driver_thread_entry(ULONG thread_input)
{
  UNUSED(thread_input);
  ULONG current_flags;
  UINT ret_val;
  while (1)
  {
    ret_val = tx_event_flags_get(&rgb_driver_flags, 0xFFFFFFF, TX_OR_CLEAR, &current_flags, 50);
    
    if(ret_val == TX_SUCCESS)
    {
        check_flags(current_flags);
    }

    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
  }
}

void check_flags(ULONG flags)
{
    if(flags & RGB_SWITCH_OFF)
    {
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
    }

    if(flags & RGB_SWITCH_ON)
    {
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
    }
}