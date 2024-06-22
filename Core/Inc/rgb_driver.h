#ifndef __RGB_DRIVER_H
#define __RGB_DRIVER_H

#include "stm32wbxx_hal.h"
#include "tx_api.h"

#define RGB_SWITCH_OFF			(1 << 0)
#define RGB_SWITCH_ON			(1 << 1)
#define RGB_ACTION_REQUEST		(1 << 2)
#define RGB_LVL_CHG_REQUEST		(1 << 3)
#define RGB_TRANSFER_REQUEST	(1 << 4)

struct XY
{
	uint16_t x;
	uint16_t y;
};
struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

enum RGB_mode_t
{
	RGB_MODE_STATIC,
	RGB_MODE_CYCLIC_GR_FAST,	//cyclic group fast
	RGB_MODE_CYCLIC_GR_SLOW,	//cyclic group slow
	RGB_MODE_CYCLIC_ALL_FAST,	//cyclic all fast
	RGB_MODE_CYCLIC_ALL_SLOW,	//cyclic all slow
	RGB_MODE_RANDOM_GR_FAST,	// random group fast
	RGB_MODE_RANDOM_GR_SLOW,	// random group slow
	RGB_MODE_RANDOM_ALL_FAST,	// random all fast
	RGB_MODE_RANDOM_ALL_SLOW,	// random all slow
	RGB_NUMBER_OF_MODES
};

struct RGB_Params_t
{
	uint8_t isOn;
	enum RGB_mode_t mode;
	uint8_t currentLevel;
	uint8_t targetLevel;
	uint8_t clientLevel;		/* last client requested on level */
	uint16_t transitionTime;	/* level transition time in tens of a second */
	struct RGB color;
	struct ZbZclClusterT* cluster;
	struct ZbZclAddrInfoT* srcInfo;
	void* arg;
};

extern struct RGB_Params_t RGB_params;
extern TX_EVENT_FLAGS_GROUP rgb_driver_flags;
extern TIM_HandleTypeDef* RGB_LED_htim;
extern uint32_t RGB_LED_Channel;

extern void rgb_driver_thread_entry(ULONG thread_input);
extern struct RGB convert_xy_to_RGB(struct XY color_xy);
extern struct XY color_temperature_to_xy(uint16_t color_temperature);

#endif /* __RGB_DRIVER_H */