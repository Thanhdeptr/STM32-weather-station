#include "stm32f4xx_hal.h"
#include "touch.h"
#include "ili9341.h"
#include "calibrate.h"
#include "stdio.h"
#define COMMAND_READ_X             			0XD0
#define COMMAND_READ_Y             			0X90
#define MW_HAL_TOUCH_READ_POINTS_COUNT		10U
#define CS_ON								(HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET))
#define CS_OFF 								(HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET))
#define TOUCH_IRQ_PORT						TC_PEN_GPIO_Port
#define TOUCH_IRQ_PIN						TC_PEN_Pin

extern SPI_HandleTypeDef hspi2;

static MATRIX matrix;

//static void DrawCross(int16_t x, int16_t y, int16_t length);
static uint8_t SpiTransfer(uint8_t byte);
static bool GetPointRaw(uint16_t* x, uint16_t* y);
static bool GetPointRaw(uint16_t* x, uint16_t* y);

static uint8_t SpiTransfer(uint8_t byte)
{
	uint8_t result;

	(void)HAL_SPI_TransmitReceive(&hspi2, &byte, &result, 1U, 1000U);

	return (result);
}

//static void DrawCross(int16_t x, int16_t y, int16_t length)
//{
//	lcdFillRGB(COLOR_WHITE);
//	lcdDrawHLine(x - length / 2, x + length / 2, y, COLOR_BLACK);
//	lcdDrawVLine(x, y - length / 2, y + length / 2, COLOR_BLACK);
//    lcdSetCursor(50,150);
//	lcdPrintf("Touch centre of cross");
//}

static bool GetPointRaw(uint16_t* x, uint16_t* y)
{
	uint8_t i;
 	bool sorted;
 	uint16_t swap_value;
	uint16_t x_raw;
	uint16_t y_raw;
	uint16_t databuffer[2][MW_HAL_TOUCH_READ_POINTS_COUNT];
	uint8_t touch_count;

	if (!TouchIsTouched())
	{
		return false;
	}

	// get set of readings
	CS_ON;
	touch_count = 0U;
	do
	{
		SpiTransfer(COMMAND_READ_X);
		x_raw = (uint16_t)SpiTransfer(0U) << 8;
		x_raw |= (uint16_t)SpiTransfer(0U);
		x_raw >>= 3;

		SpiTransfer(COMMAND_READ_Y);
		y_raw = (uint16_t)SpiTransfer(0U) << 8;
		y_raw |= (uint16_t)SpiTransfer(0U);
		y_raw >>= 3;

		databuffer[0][touch_count] = x_raw;
		databuffer[1][touch_count] = y_raw;
		touch_count++;
	}
	while (TouchIsTouched() == true && touch_count < MW_HAL_TOUCH_READ_POINTS_COUNT);
	CS_OFF;

	// check that the touch was held down during all the readings
	if (touch_count != MW_HAL_TOUCH_READ_POINTS_COUNT)
	{
		return (false);
	}

	// sort the x readings
	do
	{
		sorted = true;
		for (i = 0U; i < touch_count - 1U; i++)
		{
			if(databuffer[0][i] > databuffer[0][i + 1U])
			{
				swap_value = databuffer[0][i + 1U];
				databuffer[0][i + 1U] = databuffer[0][i];
				databuffer[0][i] = swap_value;
				sorted = false;
			}
		}
	}
	while (!sorted);

	// sort the y readings
	do
	{
		sorted = true;
		for (i = 0U; i < touch_count - 1U; i++)
		{
			if (databuffer[1][i] > databuffer[1][i + 1U])
			{
				swap_value = databuffer[1][i + 1U];
				databuffer[1][i + 1U] = databuffer[1][i];
				databuffer[1][i] = swap_value;
				sorted = false;
			}
		}
	}
	while (!sorted);

	// take averaged middle 2 readings
	*x = (databuffer[0][4] + databuffer[0][5]) / 2U;
	*y = (databuffer[1][4] + databuffer[1][5]) / 2U;

	return (true);
}

bool TouchIsTouched(void)
{
	GPIO_PinState pin_state = HAL_GPIO_ReadPin(TOUCH_IRQ_PORT, TOUCH_IRQ_PIN);
	return pin_state == GPIO_PIN_RESET;
}

bool TouchGetCalibratedPoint(int16_t* x, int16_t* y)
{
	POINT_T raw_point;
	POINT_T display_point;
	uint16_t raw_x;
	uint16_t raw_y;

	// get raw reading
	if (GetPointRaw(&raw_x, &raw_y) == false)
	{
		return false;
	}

	raw_point.x= (INT_32)raw_x;
	raw_point.y = (INT_32)raw_y;

	// apply calibration matrix
	(void)getDisplayPoint(&display_point, &raw_point, &matrix);

	// range check results
	if (display_point.x > 239)
	{
		display_point.x = 239;
	}
	if (display_point.y > 319)
	{
		display_point.y = 319;
	}

	if (display_point.x < 0)
	{
		display_point.x = 0;
	}
	if (display_point.y < 0)
	{
		display_point.y = 0;
	}

	*x = (int16_t)display_point.x;
	*y = (int16_t)display_point.y;

	return true;
}

void TouchCalibrate(void)
{
//	uint16_t x;
//	uint16_t y;
	POINT_T raw_points[3];
	POINT_T display_points[3] = {{40, 40}, {200, 40}, {200, 280}};

	raw_points[0].x = 2970;
	raw_points[0].y = 670;



    /* second point */

	raw_points[1].x = 865;
	raw_points[1].y = 711;




	raw_points[2].x = 789;
	raw_points[2].y = 3239;



	(void)setCalibrationMatrix(display_points, raw_points, &matrix);
}
