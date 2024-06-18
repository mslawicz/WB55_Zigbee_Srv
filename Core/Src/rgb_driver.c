#include "rgb_driver.h"
#include "main.h"
#include <math.h>
#include <assert.h>

#define NUMBER_OF_LEDS	8
#define NUMBER_OF_BITS		NUMBER_OF_LEDS * 3 * 8 + 1	// number of LEDs * 3 colors * 8 bits plus 1 additional byte for a zero trailing pulse
#define NUMBER_OF_GROUPS	8
#define BIT_1_DUTY	52		// 52/80 * 1.25 us = 812 ns (measured 812 ns)
#define BIT_0_DUTY	26		// 26/80 * 1.25 us = 406 ns (measured 375 ns)
#define RGB_INIT_LEVEL	20
#define RGB_ON_REQUEST	1
#define RGB_OFF_REQUEST	2
#define LEVEL_CHANGE_INTERVAL_MS	50
#define LEVEL_CHANGE_INTERVAL_TICKS	MS_TO_TICKS(LEVEL_CHANGE_INTERVAL_MS)

TX_EVENT_FLAGS_GROUP rgb_driver_flags;

/* define number of LEDs in each group */
static const uint16_t GroupSize[NUMBER_OF_GROUPS] = { 1,1,1,1,1,1,1,1 };

/* RGB color array for basic colors */
static const uint8_t ColorPattern[7][3] =
{
		{255, 0, 0},    // red
		{255, 255, 0},  // yellow
		{0, 255, 0},    // green
		{0, 255, 255},  // cyan
		{0, 0, 255},    // blue
		{255, 0, 255},  // fucsia
		{255, 0, 0}     // red
};

struct RGB_Params_t RGB_params =
{
		.mode = RGB_MODE_STATIC,
		.currentLevel=0,
		.targetLevel = RGB_INIT_LEVEL,
		.clientLevel = RGB_INIT_LEVEL,
		.transitionTime = 0,
		.color = { 255, 255, 255 },
		.cluster = NULL,
		.srcInfo = NULL,
		.arg = NULL
};

TIM_HandleTypeDef* RGB_LED_htim = NULL;
uint32_t RGB_LED_Channel;
uint16_t RGB_bits[NUMBER_OF_BITS];
float RGB_current_level;	/* float value of current level for precise level changing */
float RGB_level_multiplicator;	/* level multiplicator for a single level change step */
TX_TIMER level_timer;	/* timer for slow level changing */

void check_flags(ULONG flags);
void set_RGB_LEDs(uint16_t first, uint16_t size, struct RGB RGB_value, uint8_t level);
HAL_StatusTypeDef send_RGB_data(TIM_HandleTypeDef* htim, uint32_t Channel);
void turn_off_LEDs(void);
void RGB_mode_handler(void);
void RGB_level_handler(void);
void level_change_timer_cbk(ULONG param);
void set_level_transition_parameters(void);

void rgb_driver_thread_entry(ULONG thread_input)
{
  UNUSED(thread_input);
  ULONG current_flags;
  UINT ret_val;

  tx_event_flags_create(&rgb_driver_flags, "RGB driver flags");
  ret_val = tx_timer_create(&level_timer, "level change timer", level_change_timer_cbk, 0, LEVEL_CHANGE_INTERVAL_TICKS, 0, TX_NO_ACTIVATE);
  if(ret_val != TX_SUCCESS)
  {
	Error_Handler();
  }
  /* clear all leds on startup */
  turn_off_LEDs();

  // XXX test
  RGB_params.transitionTime = 5;
  tx_event_flags_set(&rgb_driver_flags, RGB_SWITCH_ON, TX_OR);


  while (1)
  {
    ret_val = tx_event_flags_get(&rgb_driver_flags, 0xFFFFFFFF, TX_OR_CLEAR, &current_flags, TX_WAIT_FOREVER);
    
    if(ret_val == TX_SUCCESS)
    {
        check_flags(current_flags);
    }

    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);	//XXX test
  }
}

void check_flags(ULONG flags)
{
    if(flags & RGB_SWITCH_OFF)
    {
		RGB_params.targetLevel = 0;
		set_level_transition_parameters();
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);  //XXX test
		/* initiate action on next pass */
        tx_event_flags_set(&rgb_driver_flags, RGB_LVL_CHG_REQUEST, TX_OR);
    }

    if(flags & RGB_SWITCH_ON)
    {
		RGB_params.targetLevel = RGB_params.clientLevel;
		set_level_transition_parameters();
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);  //XXX test
		/* initiate action on next pass */
		tx_event_flags_set(&rgb_driver_flags, RGB_LVL_CHG_REQUEST, TX_OR);
    }

	if(flags & RGB_ACTION_REQUEST)
	{
		RGB_mode_handler();
	}

	if(flags & RGB_LVL_CHG_REQUEST)
	{
		RGB_level_handler();
	}
}

void RGB_mode_handler(void)
{
	switch(RGB_params.mode)
	{
		case RGB_MODE_STATIC:
		/* set static color for all LEDs */
        set_RGB_LEDs(0, NUMBER_OF_LEDS, RGB_params.color, RGB_params.currentLevel);	//set all LEDs to current global color and level
        send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
		/* one-shot action - no next steps needed */
		break;

		default:
		turn_off_LEDs();
		break;
	}
}

void RGB_level_handler(void)
{
	uint8_t previous_level = RGB_params.currentLevel;

	if(RGB_params.transitionTime == 0)
	{
		/* single-step transition */
		RGB_params.currentLevel = RGB_params.targetLevel;
	}

	if(RGB_params.currentLevel != RGB_params.targetLevel)
	{
		/* apply one step of level adjustment */
		assert(RGB_level_multiplicator > 1.0f);
		assert(RGB_current_level > 0.0f);

		if(RGB_params.currentLevel > RGB_params.targetLevel)
		{
			/* decrease level */
			RGB_current_level /= RGB_level_multiplicator;
			RGB_params.currentLevel = (uint8_t)RGB_current_level;
			if(RGB_params.currentLevel < RGB_params.targetLevel)
			{
				/* current level cannot go below target level */
				RGB_params.currentLevel = RGB_params.targetLevel;
			}
		}
		else
		{
			/* increase level */
			RGB_current_level *= RGB_level_multiplicator;
			/* use 16-bit value for overflow prevention */
			uint16_t new_level = (uint16_t)RGB_current_level;
			if(new_level > RGB_params.targetLevel)
			{
				/* current level cannot go above target level */
				RGB_params.currentLevel = RGB_params.targetLevel;
			}
			else
			{
				RGB_params.currentLevel = (uint8_t)new_level;
			}
		}

		if(RGB_params.currentLevel != RGB_params.targetLevel)
		{
			/* not finished yet - request next pass */
			tx_timer_activate(&level_timer);
		}
	}

	if(RGB_params.currentLevel != previous_level)
	{
		/* the level has been changed - refresh LEDs */
		RGB_mode_handler();
	}
}

//convert color data from xy space to RGB value
void convert_xy_to_RGB(uint16_t x, uint16_t y, struct RGB* pRGB)
{
#define constrain_from_0(x)		if(x < 0) { x = 0.0; }
	float gamma_correction(float  vat2correct);

	const float ZXY2RGB[3][3] =	//matrix for converting Ikea light bulb color XY ( CIE 1931 colorspace ) to RGB
	{
			{ 1.656, -0.355, -0.255 },
			{ -0.707, 1.655, 0.036 },
			{ 0.052, -0.121, 1.012 }
	};
	const float XY_NORM = 1.0 / 65536.0;

	/* normalize x y z to 0..1 range */
	const float x_n = x * XY_NORM;
	const float y_n = y * XY_NORM;
	const float z_n = 1.0 - x_n - y_n;

	/* calculate CIE X Y Z values */
	const float Y = 1.0;	//brightness value for calculations
	const float X = Y / y_n * x_n;
	const float Z = Y / y_n * z_n;

	/* calculate r g b color values (not normalized) */
	float r = X * ZXY2RGB[0][0] + Y * ZXY2RGB[0][1] + Z * ZXY2RGB[0][2];
	float g = X * ZXY2RGB[1][0] + Y * ZXY2RGB[1][1] + Z * ZXY2RGB[1][2];
	float b = X * ZXY2RGB[2][0] + Y * ZXY2RGB[2][1] + Z * ZXY2RGB[2][2];

	/* find maximum of r g b values */
	float rgb_max = r > g ? r : g;
	rgb_max = b > rgb_max ? b : rgb_max;

	/* normalize r g b to 0..1 range */
	if(rgb_max > 1.0)
	{
		r /= rgb_max;
		g /= rgb_max;
		b /= rgb_max;
	}

	constrain_from_0(r)
	constrain_from_0(g)
	constrain_from_0(b)

	/* apply gamma correction */
	r = gamma_correction(r);
	g = gamma_correction(g);
	b = gamma_correction(b);

	/* normalize to 0..255 */
	pRGB->R = (uint8_t)(r * 255 + 0.5);
	pRGB->G = (uint8_t)(g * 255 + 0.5);
	pRGB->B = (uint8_t)(b * 255 + 0.5);
}

//adjust gamma correction to a color value
float gamma_correction(float val2correct)
{
	if(val2correct <= 0.0031308)
	{
		return val2correct * 12.92;
	}

	return 1.055 * powf(val2correct, 0.416666) - 0.055;
}

//adjust color value to the desired level
uint8_t apply_level(uint8_t value, uint8_t level)
{
	return value * level / 255;
}

//set RGB LED PWM pulse data to the array
void set_RGB_bits(uint16_t LED ,struct RGB value, uint8_t level)
{
	assert(LED < NUMBER_OF_LEDS);
	uint16_t* pBuffer = RGB_bits + LED * 3 * 8;
	uint8_t bit;
	uint8_t colorValue;

	//data must be sent in GRB order
	colorValue = apply_level(value.G, level);
	for(bit = 0; bit <8; bit++)	// place red bits
	{
		*pBuffer++ = ((colorValue >> (7-bit)) & 1) ? BIT_1_DUTY : BIT_0_DUTY;
	}

	colorValue = apply_level(value.R, level);
	for(bit = 0; bit <8; bit++)	// place green bits
	{
		*pBuffer++ = ((colorValue >> (7-bit)) & 1) ? BIT_1_DUTY : BIT_0_DUTY;
	}

	colorValue = apply_level(value.B, level);
	for(bit = 0; bit <8; bit++)	// place blue bits
	{
		*pBuffer++ = ((colorValue >> (7-bit)) & 1) ? BIT_1_DUTY : BIT_0_DUTY;
	}
}

//send all RGB LED data to LED devices
HAL_StatusTypeDef send_RGB_data(TIM_HandleTypeDef* htim, uint32_t Channel)
{
	RGB_bits[NUMBER_OF_BITS - 1] = 0;		// the last pulse to be sent must be a PWM zero pulse
	// send all RGB bits followed by a zero pulse
	return HAL_TIM_PWM_Start_DMA(htim, Channel, (uint32_t*)RGB_bits, NUMBER_OF_BITS);
}

// set a number of LEDs to a certain color and level
void set_RGB_LEDs(uint16_t first, uint16_t size, struct RGB RGB_value, uint8_t level)
{
	uint16_t LED_idx;

	for(LED_idx = first; LED_idx < first + size; LED_idx++)
	{
		set_RGB_bits(LED_idx, RGB_value, level);
	}
}

//turns off all LEDs without changing parameters
void turn_off_LEDs(void)
{
	const struct RGB RGB_off = { 0, 0, 0 };
	set_RGB_LEDs(0, NUMBER_OF_LEDS, RGB_off, 0);	//set all LEDs to off state
	send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
}

//mode of cyclically changing colors in groups
//use_groups: different colors in groups (true) or the same color for all groups (false)
//noOfSteps: number of steps in the cycle
void RGB_cyclic_change(uint8_t use_groups, uint32_t noOfSteps)
{
	static uint32_t step = 0;		//current step index
	struct RGB RGB_value;

	uint8_t group;
	uint16_t groupIdx = 0;
	for(group = 0; group < NUMBER_OF_GROUPS; group++)
	{
		float phase = (float)step / noOfSteps + (use_groups ? (float)group / NUMBER_OF_GROUPS : 0.0);
		phase -= (int)phase;		//leave only the fractional part of phase
		float patternPhase = phase * 6;	//floating index for the ColorPattern array
		uint8_t lowerIdx = (int)patternPhase;
		RGB_value.R = ColorPattern[lowerIdx][0] + (int)((ColorPattern[lowerIdx + 1][0] - ColorPattern[lowerIdx][0]) * (patternPhase - lowerIdx));
		RGB_value.G = ColorPattern[lowerIdx][1] + (int)((ColorPattern[lowerIdx + 1][1] - ColorPattern[lowerIdx][1]) * (patternPhase - lowerIdx));
		RGB_value.B = ColorPattern[lowerIdx][2] + (int)((ColorPattern[lowerIdx + 1][2] - ColorPattern[lowerIdx][2]) * (patternPhase - lowerIdx));
		set_RGB_LEDs(groupIdx, GroupSize[group], RGB_value, RGB_params.currentLevel);	//set all LEDs in the group
		groupIdx += GroupSize[group];		//set start index of the next group
	}

	send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
	step = (step + 1) % noOfSteps;		//next cycle step in the next function call
}

//generates new RGB values several times and returns the most color-saturated one
void getNewRGB(struct RGB* pValue)
{
	struct RGB newRGB;
	uint8_t testNr;
	uint8_t min, max, span;
	uint8_t spanMax = 0;

	for(testNr = 0; testNr < 10; testNr++)
	{
		newRGB.R = rand() % 0x100;
		newRGB.G = rand() % 0x100;
		newRGB.B = rand() % 0x100;
		min = (newRGB.R < newRGB.G) ? newRGB.R :  newRGB.G;
		min = (min < newRGB.B) ? min :  newRGB.B;
		max = (newRGB.R > newRGB.G) ? newRGB.R :  newRGB.G;
		max = (max > newRGB.B) ? max :  newRGB.B;
		span = max - min;
		if(span > spanMax)
		{
			spanMax = span;
			*pValue = newRGB;
		}
	}
}

//mode of randomly changing colors in groups
//use_groups: different colors in groups (TRUE) or the same color for all groups (FALSE)
//noOfSteps: number of steps in a single change action
void RGB_random_change(uint8_t use_groups, uint32_t noOfSteps)
{
	static struct RGB RGB_value[NUMBER_OF_GROUPS][2];		//previous and next RGB values
	static uint8_t activeGroup = 0;
	static uint32_t currentStep = 0;
	static uint8_t firstPass = TRUE;

	uint8_t group;
	uint16_t groupIdx = 0;
	struct RGB currentValue;
	struct RGB targetValue;

	if(firstPass)
	{
		srand(RGB_LED_htim->Instance->CNT);
		targetValue.R = rand() % 0x100;
		targetValue.G = rand() % 0x100;
		targetValue.B = rand() % 0x100;
		for(group = 0; group < NUMBER_OF_GROUPS; group++)
		{
			RGB_value[group][1] = RGB_value[group][0] = targetValue;
		}
		set_RGB_LEDs(0, NUMBER_OF_LEDS, targetValue, RGB_params.currentLevel);	//set all LEDs to random value
		firstPass = FALSE;
	}

	if(currentStep == 0)
	{
		//set next target RGB value
		getNewRGB(&targetValue);
		do
		{
			group = rand() % NUMBER_OF_GROUPS;
		} while((NUMBER_OF_GROUPS > 1) && (group == activeGroup));
		activeGroup = group;
	}

	for(group = 0; group < NUMBER_OF_GROUPS; group++)
	{

		if(!use_groups || (group == activeGroup))
		{
			if(currentStep == 0)
			{
				RGB_value[group][0] = RGB_value[group][1];
				RGB_value[group][1] = targetValue;
			}

			currentValue.R = (uint8_t)((int)RGB_value[group][0].R + ((int)RGB_value[group][1].R - (int)RGB_value[group][0].R) * (int)currentStep / (int)noOfSteps);
			currentValue.G = (uint8_t)((int)RGB_value[group][0].G + ((int)RGB_value[group][1].G - (int)RGB_value[group][0].G) * (int)currentStep / (int)noOfSteps);
			currentValue.B = (uint8_t)((int)RGB_value[group][0].B + ((int)RGB_value[group][1].B - (int)RGB_value[group][0].B) * (int)currentStep / (int)noOfSteps);
			set_RGB_LEDs(groupIdx, GroupSize[group], currentValue, RGB_params.currentLevel);	//set all LEDs in the group
		}
		groupIdx += GroupSize[group];		//set start index of the next group
	}

	send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
	if(++currentStep >= noOfSteps)
	{
		currentStep = 0;
	}
}

void level_change_timer_cbk(ULONG param)
{
	UNUSED(param);
	tx_event_flags_set(&rgb_driver_flags, RGB_LVL_CHG_REQUEST, TX_OR);
}

void set_level_transition_parameters(void)
{
	if((RGB_params.transitionTime != 0) && (RGB_params.currentLevel != RGB_params.targetLevel))
	{
		uint16_t numb_trans_steps = 100 * RGB_params.transitionTime / LEVEL_CHANGE_INTERVAL_MS;
		if(numb_trans_steps == 0)
		{
			/* number of transition steps must be > 0 */
			numb_trans_steps = 1;
		}
		uint8_t delta = (RGB_params.currentLevel > RGB_params.targetLevel) ?
						RGB_params.currentLevel - RGB_params.targetLevel :
						RGB_params.targetLevel - RGB_params.currentLevel;
		RGB_level_multiplicator = powf((float)delta, 1.0f / (float)numb_trans_steps);
		RGB_current_level = (float)RGB_params.currentLevel;
		if(RGB_current_level < 1.0f)
		{
			/* this variable must not be 0 on startup of transition */
			RGB_current_level = 1.0f;
		}					
	}
}