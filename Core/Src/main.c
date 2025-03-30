/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sensor.h"
#include "ili9341.h"
#include "image.h"
#include "touch.h"
#include "fonts.h"
#include "string.h"
//#include "ili9341_touch.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define min(a,b) (((a)<(b))?(a):(b))
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
float temp_dht;
float humid_dht;
char dhtTemp[10];
char dhtHump[10];
char apiTemp[10];
char apiIcon[5];
char apiDate[15];
char buf[10];
char buf1[10];
char buf2[20];
uint8_t choosecity;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int i = 0;

struct dht_data
{						//GET_TIME; Lua thoi gian
	char citycode[20];
};
struct dht_data data_trant;


struct data_uart
{
	float temp;
	int icon;
	char date[15];
};
struct data_uart data_receive;
char hub[6] = {};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart2;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_FSMC_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
float temp;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2 )
	{
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&data_receive, sizeof(data_receive));
}
}
void Hienthithoitiet(int city){
	choosecity = 0;
	lcdFillRGB(COLOR_BLACK);
	lcdDrawRect(10, 10, 220, 300, COLOR_GREEN);
	lcdDrawLine(10, 50, 229, 50, COLOR_GREEN);
	lcdDrawLine(10, 100, 229, 100, COLOR_GREEN);
	lcdDrawLine(120, 100, 120, 309, COLOR_GREEN);
	lcdFillTriangle(200, 65, 200, 85, 215, 76, COLOR_WHITE);
	switch(city){
			case 1:

				sprintf(buf2, "%s", "3-433307_1_AL");
				lcdPrintCenteredInRect("HO CHI MINH", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 2:
	        	sprintf(buf2, "%s", "213490");
	        	lcdPrintCenteredInRect("ROME", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 3:

	        	sprintf(buf2, "%s", "226396");
	        	lcdPrintCenteredInRect("TOKYO", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 4:
	        	sprintf(buf2, "%s", "22889");
	        	lcdPrintCenteredInRect("SYDNEY", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 5:
	        	sprintf(buf2, "%s", "353412");
	        	lcdPrintCenteredInRect("HANOI", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 6:

	        	sprintf(buf2, "%s", "178087");
	        	lcdPrintCenteredInRect("BERLIN", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 7:
	        	sprintf(buf2, "%s", "328328");
	        	lcdPrintCenteredInRect("LONDON", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	        case 8:
	        	sprintf(buf2, "%s", "623");
	        	lcdPrintCenteredInRect("PARIS", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	            break;
	}
	sprintf(data_trant.citycode, "%s", buf2);
	HAL_UART_Transmit(&huart2, (uint8_t *) &data_trant, sizeof(data_trant),1000);
}

void reverseDate(const char* inputDate, char* outputDate) {
    // Khai báo các biến để tách ngày, tháng, năm
    char year[5], month[3], day[3];

    // Tách năm, tháng, ngày từ chuỗi inputDate
    strncpy(year, inputDate, 4);  // Năm
    year[4] = '\0';  // Kết thúc chuỗi

    strncpy(month, inputDate + 5, 2);  // Tháng
    month[2] = '\0';  // Kết thúc chuỗi

    strncpy(day, inputDate + 8, 2);  // Ngày
    day[2] = '\0';  // Kết thúc chuỗi

    // Kết hợp lại theo định dạng "DD-MM-YYYY"
    sprintf(outputDate, "%s-%s-%s", day, month, year);
}

void CheckWeatherCondition(int i) {
    if ((i >= 1 && i <= 6) || i == 20 || i == 21 || i == 30) {
        lcdDrawImage(138, 200, &bmsunnycloud); // Hình ảnh nắng
    } else if ((i >= 7 && i <= 11) || (i >= 31 && i <= 38)) {
        lcdDrawImage(138, 200, &bmcloudy_icon); // Hình ảnh mây
    } else if ((i >= 12 && i <= 18) || (i >= 39 && i <= 42)) {
        lcdDrawImage(138, 200, &bmsnowy_icon); // Hình ảnh mưa
    } else if ((i >= 22 && i <= 32) || (i >= 43 && i <= 44)) {
        lcdDrawImage(138, 200, &bmthunder_icon); // Hình ảnh tuyết
    } else {
        lcdDrawImage(138, 200, &bmcloudy_icon);
    }
}

void ChooseCityScreen(){
	choosecity = 1; //dang o man hinh chon thanh pho
	lcdFillRGB(COLOR_BLACK);
	  //lcdDrawImage(0, 0, &bmSTLogo);
	  lcdDrawRect(10, 10, 220, 300, COLOR_GREEN);
	  lcdDrawLine(10, 50, 229, 50, COLOR_GREEN);
	  //lcdSetTextFont(&Font12);

	  lcdPrintCenteredInRect("Choose City", 10, 10, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	  lcdPrintCenteredInRect("SAIGON", 10, 50, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("HANOI", 110, 50, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("ROME", 10, 100, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("BERLIN", 110, 100, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("TOKYO", 10, 150, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("LONDON", 110, 150, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("SYDNEY", 10, 200, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
	  lcdPrintCenteredInRect("PARIS", 110, 200, 110, 50, &Font20, COLOR_BLACK, COLOR_WHITE);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//TouchScreen_Coordinates ;
	int16_t xa = 0;
	int16_t ya = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM10_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_FSMC_Init();
  MX_SPI2_Init();
  LCD_BL_ON();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim10);
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&data_receive, sizeof(data_receive));
  lcdInit();
  lcdSetOrientation(0);
  lcdDisplayOn();
  lcdFillRGB(COLOR_BLACK);
  TouchCalibrate();

  ChooseCityScreen();


  //lcdPrintCenteredInRect(apiTemp, 120, 100, 110, 105, &Font20, COLOR_WHITE, COLOR_BLACK);

  //lcdDrawImage(138,200, &bmsunnycloud);



//  lcdPrintf("hello");
 // HAL_Delay(2000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(TouchGetCalibratedPoint(&xa, &ya))
		{

		  i++;

		  sprintf(buf, "%06i", xa);
		  //lcdprstr(20,265,buf,&Font16, COLOR_BLACK, COLOR_WHITE); // ham nay t viet in string tai toa do do mk chon
		  sprintf(buf1, "%06i", ya);
		  //lcdprstr(20,285,buf1,&Font16,  COLOR_BLACK, COLOR_WHITE);

		  if(choosecity){
		  if(5 < xa && xa < 100 && 55 < ya && ya < 90 )
					  {
						  //lcdDrawImage(0, 0, &bmanhmungtho4);
			  	  	  	  Hienthithoitiet(1);
					  }
		  if(5 < xa && xa < 100 && 110 < ya && ya < 150 )
		  			  {
		  				  Hienthithoitiet(2);
		  			  }
		  if(5 < xa && xa < 100 && 160 < ya && ya < 205 )
		  			  {
		  				  Hienthithoitiet(3);
		  			  }
		  if(5 < xa && xa < 100 && 210 < ya && ya < 240 )
		  			  {
		  				  Hienthithoitiet(4);
		  			  }
		  if(115 < xa && xa < 225 && 60 < ya && ya < 82 )
					  {
						  Hienthithoitiet(5);
					  }
		  if(115 < xa && xa < 225 && 115 < ya && ya < 135 )
		  			  {
		  				  Hienthithoitiet(6);
		  			  }
		  if(115 < xa && xa < 225 && 168 < ya && ya < 190 )
		  			  {
		  				  Hienthithoitiet(7);
		  			  }
		  if(115 < xa && xa < 225 && 215 < ya && ya < 235 )
		  			  {
		  				  Hienthithoitiet(8);
		  			  }
		  }else{
			  if(200 < xa && xa < 230 && 55 < ya && ya < 95 ){
			  	        		  HAL_UART_Transmit(&huart2, (uint8_t *) &data_trant, sizeof(data_trant),1000);
			  	        	  }
			  if(0 < xa && xa < 220 && 0 < ya && ya < 50 ){
			  			  	       ChooseCityScreen();
			  			  	  }
		  }

		}
	  	  	  DHT11_GetData();
	  	  	  temp_dht = DHT11_Gettemp();
	  	  	  humid_dht = DHT11_Gethumid();
	  	  	  sprintf(dhtTemp, "%.2f*C", Temperature);
	  	  	  sprintf(dhtHump, "%.2f%%", Humidity);
	          sprintf(apiTemp, "%.2f*F", data_receive.temp);
	          sprintf(apiIcon, "%i", data_receive.icon);
	          reverseDate(data_receive.date, apiDate);

	          if(!choosecity){
	        	  lcdPrintCenteredInRect(apiTemp, 120, 100, 110, 105, &Font20, COLOR_WHITE, COLOR_BLACK);
	        	  //lcdprstr( 125, 255, data_trant.citycode ,&Font16,  COLOR_BLACK, COLOR_WHITE);
	        	  lcdPrintCenteredInRect(apiDate, 10, 50, 220, 50, &Font20, COLOR_WHITE, COLOR_BLACK);
	        	  lcdPrintCenteredInRect(dhtHump, 10, 100, 110, 105, &Font20, COLOR_WHITE, COLOR_BLACK);
	        	  lcdPrintCenteredInRect(dhtTemp, 10, 205, 110, 104, &Font20, COLOR_WHITE, COLOR_BLACK);
	        	  CheckWeatherCondition(data_receive.icon);

	          }
	  //i++;
	 //HAL_Delay(1000);
	 //DHT11_Start ();
	 //checker = DHT11_Check_Response();
	  //DHT11_Read()


	 HAL_Delay(200);
	 // while(cont < 200)
	  //{
	//  DHT_Delay (10000);
	//  cont++;
	//  }
	 // cont = 0;
   // HAL_Delay(2000);
	 HAL_Delay(400);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 83;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 167;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 65534;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|TOUCH_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : TC_PEN_Pin */
  GPIO_InitStruct.Pin = TC_PEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TC_PEN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TOUCH_CS_Pin */
  GPIO_InitStruct.Pin = TOUCH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOUCH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 6;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 6;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
