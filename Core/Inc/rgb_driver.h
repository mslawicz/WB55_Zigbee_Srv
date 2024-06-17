#ifndef __RGB_DRIVER_H
#define __RGB_DRIVER_H

#include "stm32wbxx_hal.h"
#include "tx_api.h"

#define RGB_SWITCH_OFF    	(1 << 0)
#define RGB_SWITCH_ON     	(1 << 1)
#define RGB_ACTION_REQUEST  (1 << 2)

struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

enum RGB_mode_t
{
	RGB_MODE_STATIC,
	// Mode_CyclingGroupsFast,
	// Mode_CyclingGroupsSlow,
	// Mode_CyclingAllFast,
	// Mode_CyclingAllSlow,
	// Mode_RandomGroupsFast,
	// Mode_RandomGroupsSlow,
	// Mode_RandomAllFast,
	// Mode_RandomAllSlow,
	RGB_NUMBER_OF_MODES
};

struct RGB_Params_t
{
	enum RGB_mode_t mode;
	uint8_t onOffRequest;	/* 0, RGB_OFF_REQUEST, RGB_ON_REQUEST */
	uint8_t currentLevel;
	//uint8_t targetLevel;
	//uint32_t transitionSteps;
	struct RGB color;
	struct ZbZclClusterT* cluster;
	struct ZbZclAddrInfoT* srcInfo;
	void* arg;
};

extern TX_EVENT_FLAGS_GROUP rgb_driver_flags;
extern TIM_HandleTypeDef* RGB_LED_htim;
extern uint32_t RGB_LED_Channel;

extern void rgb_driver_thread_entry(ULONG thread_input);

#endif /* __RGB_DRIVER_H */