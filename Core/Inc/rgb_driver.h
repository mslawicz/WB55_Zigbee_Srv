#ifndef __RGB_DRIVER_H
#define __RGB_DRIVER_H

#include "stm32wbxx_hal.h"
#include "tx_api.h"

#define RGB_SWITCH_OFF    (1 << 0)
#define RGB_SWITCH_ON     (1 << 1)

extern TX_EVENT_FLAGS_GROUP rgb_driver_flags;

extern void rgb_driver_thread_entry(ULONG thread_input);

#endif /* __RGB_DRIVER_H */