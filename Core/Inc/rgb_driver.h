#ifndef __RGB_DRIVER_H
#define __RGB_DRIVER_H

#include "stm32wbxx_hal.h"
#include "tx_api.h"

#define RGB_SWITCH_OFF    (1 << 0)
#define RGB_SWITCH_ON     (1 << 1)

struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct RGB_Params_t
{
	//uint8_t OnOff;
	uint8_t currentLevel;
	//uint8_t targetLevel;
	//uint32_t transitionSteps;
	struct RGB color;
	struct ZbZclClusterT* cluster;
	//enum RGB_mode_t mode;
	struct ZbZclAddrInfoT* srcInfo;
	void* arg;
};

extern TX_EVENT_FLAGS_GROUP rgb_driver_flags;
extern TIM_HandleTypeDef* RGB_LED_htim;
extern uint32_t RGB_LED_Channel;

extern void rgb_driver_thread_entry(ULONG thread_input);

#endif /* __RGB_DRIVER_H */