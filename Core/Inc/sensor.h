#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "stdio.h"

extern TIM_HandleTypeDef htim10;

void DHT_Delay(uint16_t time)
{
	/* change your code here for the DHT_Delay in microseconds */
	__HAL_TIM_SET_COUNTER(&htim10, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim10))<time);
}

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*********************************** DHT11 FUNCTIONS ********************************************/

#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_1

void DHT11_Start (void)
{
	Set_Pin_Output (DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	DHT_Delay (18000);   // wait for 18ms
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1); // pull the pin high   // wait for 20us
	//DHT_Delay (20);
	Set_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	DHT_Delay (20);
	if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	{
		DHT_Delay (80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = 2; // 255
		//flag = 1;
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
     	while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		DHT_Delay (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1

		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));
		//while (flag == 0);  // wait for the pin to go low
	}
	return i;
}

void DHT11_GetData(){
	  DHT11_Start();
	  Presence = DHT11_Check_Response();
	  Rh_byte1 = DHT11_Read ();
	  Rh_byte2 = DHT11_Read ();
	  Temp_byte1 = DHT11_Read ();
	  Temp_byte2 = DHT11_Read ();
	  SUM = DHT11_Read();

	 // TEMP = Temp_byte1;
	 // RH = Rh_byte1;

	  Temperature = (float) Temp_byte1 + (float)(Temp_byte2 / 10.0);
	  Humidity = (float)Rh_byte1 + (float)(Rh_byte2 / 10.0);
	  //DHT_Delay(2000000);
}
float DHT11_Gettemp()
{
	return Temperature;
}
float DHT11_Gethumid()
{
	return Humidity;
}
void DHT11_WriteData(){
	char ADC_Temp[10];
	char ADC_Hump[10];

	sprintf(ADC_Temp, "%.2f*C", Temperature);
	sprintf(ADC_Hump, "%.2f%%", Humidity);

	//ILI9341_WriteString(233, 80, ADC_Temp, Font_11x18, ILI9341_BLACK, 0x8E7D);
	//ILI9341_WriteString(233, 125, ADC_Hump, Font_11x18, ILI9341_BLACK, 0x8E7D);
}

#endif
