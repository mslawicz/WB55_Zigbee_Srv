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
		//.OnOff = 0,
		.currentLevel=RGB_INIT_LEVEL,
		//.targetLevel = RGB_INIT_LEVEL,
		//.transitionSteps = 0,
		.color = { 255, 255, 255 },
		//.mode = Mode_Static,
		.cluster = NULL,
		.srcInfo = NULL,
		.arg = NULL
};

TIM_HandleTypeDef* RGB_LED_htim = NULL;
uint32_t RGB_LED_Channel;
uint16_t RGB_bits[NUMBER_OF_BITS];

void check_flags(ULONG flags);
void set_RGB_LEDs(uint16_t first, uint16_t size, struct RGB RGB_value, uint8_t level);
HAL_StatusTypeDef send_RGB_data(TIM_HandleTypeDef* htim, uint32_t Channel);
void turn_off_LEDs(void);

void rgb_driver_thread_entry(ULONG thread_input)
{
  UNUSED(thread_input);
  ULONG current_flags;
  UINT ret_val;

  tx_event_flags_create(&rgb_driver_flags, "RGB driver flags");

  while (1)
  {
    ret_val = tx_event_flags_get(&rgb_driver_flags, 0xFFFFFFFF, TX_OR_CLEAR, &current_flags, 10);
    
    if(ret_val == TX_SUCCESS)
    {
        check_flags(current_flags);
    }

    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    const struct RGB test_RGB = {0x03, 0x30, 0xAA};
    set_RGB_LEDs(0, NUMBER_OF_LEDS, test_RGB, 255);	//set all LEDs to current global color and level
    send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
  	//HAL_TIM_PWM_Start_DMA(RGB_LED_htim, RGB_LED_Channel, (uint32_t*)test_buf, 15);  
  }
}

void check_flags(ULONG flags)
{
    if(flags & RGB_SWITCH_OFF)
    {
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);  //XXX test
        turn_off_LEDs();
    }

    if(flags & RGB_SWITCH_ON)
    {
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);  //XXX test
        set_RGB_LEDs(0, NUMBER_OF_LEDS, RGB_params.color, RGB_params.currentLevel);	//set all LEDs to current global color and level
        send_RGB_data(RGB_LED_htim, RGB_LED_Channel);	//send data to RGB LED units
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