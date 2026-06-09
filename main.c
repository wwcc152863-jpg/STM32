/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ESP8266.h"
#include "delay.h"
#include "OLED.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "DS18B20.h"
#include "MOTOR.h"
#include "KEY.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern struct timer residue_time;
uint8_t motor_dri_flag;

uint8_t mode = 0;
float temp_value;
char s[100];
uint8_t water_level, light, turbidity;
uint8_t water_level_min = 20, water_level_flag, temp_min = 10, light_min = 50, wtl_i, time_i, dev_i;
uint8_t water_flag, led_flag, oxy_flag, warm_flag, weishi_flag, motor_flag;
uint8_t time_flag;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
*	功能：设置加热装置状态
*	参数：1开  0关
*	返回值：无
*/
void Warm_Set_State(uint8_t x)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, (GPIO_PinState)x);
}

/*
*	功能：设置水泵状态
*	参数：1开  0关
*	返回值：无
*/
void Water_Set_State(uint8_t x)
{
	water_level_flag = x;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (GPIO_PinState)x);
}

/*
*	功能：设置LED台灯状态
*	参数：1开  0关
*	返回值：无
*/
void LED_Set_State(uint8_t x)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, (GPIO_PinState)x);
}

/*
*	功能：设置增氧装置状态
*	参数：1开  0关
*	返回值：无
*/
void OXY_Set_State(uint8_t x)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, (GPIO_PinState)x);
}
/*
*	功能：设置蜂鸣器状态
*	参数：1开  0关
*	返回值：无
*/
void Buzzer_Set_State(uint8_t x)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (GPIO_PinState)x);
}
/*
*	功能：获取adc的值
*	参数：无
*	返回值：无
*/
void ADC_GetValue(void)
{
	uint32_t adc_sum[3] = {0};
	uint16_t adc_value[3] = {0};
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 3);
		adc_sum[0] += adc_value[0];
		adc_sum[1] += adc_value[1];
		adc_sum[2] += adc_value[2];
	}
	light = 100 - (adc_sum[0] / 10 / 4095.0 * 100 );
	water_level = adc_sum[1] / 10 / 4095.0 * 100;
	turbidity = 100 - adc_sum[2] / 10 / 4095.0 * 100;
}

/*
*	功能：控制逻辑
*	参数：无
*	返回值：无
*/
uint16_t weishi_time;
void Ctrl(void)
{
	if (mode == 0)
	{
		(water_level < water_level_min) ? Water_Set_State(1) : Water_Set_State(0);
		((int)temp_value < temp_min) ? Warm_Set_State(1) : Warm_Set_State(0);
		(light < light_min) ? LED_Set_State(1) : LED_Set_State(0);
		if ((water_level < water_level_min) || (temp_value < temp_min) || (light < light_min))
			Buzzer_Set_State(1);
		else
			Buzzer_Set_State(0);
	}
	else if (mode == 2)
	{
		LED_Set_State(led_flag);
		Water_Set_State(water_flag);
		Warm_Set_State(warm_flag);
	}
	
	if (weishi_flag == 1)
	{
		if ((motor_dri_flag == 1) && (weishi_time < 100))
		{
			weishi_time++;
			return;
		}
		else if (motor_dri_flag == 0)
		{
			MOTOR_Start(1);
			motor_dri_flag = 1;
			return;
		}
		weishi_time = 0;
		MOTOR_Start(0);
		motor_dri_flag = 0;
		motor_flag = weishi_flag = 0;
	}
	
	if (time_flag == 1)
	{
		if ((residue_time.hour == 0) && (residue_time.minute == 0) && (residue_time.sec == 0))
		{
			weishi_flag = motor_flag;
			OXY_Set_State(oxy_flag);
			HAL_TIM_Base_Stop_IT(&htim3);
			time_flag = 0;
		}
	}
}
/*
*	功能：页面显示
*	参数：无
*	返回值：无
*/
void Page(void)
{
	OLED_ShowChinese(1, 1, (mode == 0) ? "自动" : (mode == 1) ? "阈值" : (mode == 2) ? "控制" : "设置");
	if (mode == 0)
	{
		OLED_ShowChinese(1, 16, "温度：");
		sprintf(s, "%.1f ", temp_value);
		OLED_ShowString(40, 16, s, 8);
		OLED_ShowChinese(65, 48, "光照：");
		sprintf(s, "%d  ", light);
		OLED_ShowString(105, 48, s, 8);
		
		OLED_ShowChinese(1, 32, "水位：");
		OLED_ShowChinese(40, 32, (water_level_flag == 1) ? "没水" : "有水");
		
		OLED_ShowChinese(1, 48, "浊度：");
		sprintf(s, "%d ", turbidity);
		OLED_ShowString(40, 48, s, 8);
	}
	else if (mode == 1)
	{
		OLED_ShowChinese(105, 16 * (wtl_i + 1), "←");
		OLED_ClearArea(105, (16 * (wtl_i + 1) - 16) == 0 ?  48 : (16 * (wtl_i + 1) - 16), 16, 16);
		OLED_ShowChinese(1, 32, "温度最小：");
		sprintf(s, "%d ", temp_min);
		OLED_ShowString(80, 32, s, 8);
		OLED_ShowChinese(1, 48, "光照最小：");
		sprintf(s, "%d ", light_min);
		OLED_ShowString(80, 48, s, 8);
		OLED_ShowChinese(1, 16, "水位最小：");
		sprintf(s, "%d ", water_level_min);
		OLED_ShowString(80, 16, s, 8);
	}
	else if (mode == 2)
	{
		OLED_ShowChinese(60, 16 * (dev_i + 1), "←");
		OLED_ClearArea(60, (16 * (dev_i + 1) - 16) == 0 ?  48 : (16 * (dev_i + 1) - 16), 16, 16);
		OLED_ShowChinese(1, 16, "水泵：");
		OLED_ShowChinese(40, 16, (water_flag == 1) ? "开" : "关");
		OLED_ShowChinese(1, 32, "加热：");
		OLED_ShowChinese(40, 32, (warm_flag == 1) ? "开" : "关");
		OLED_ShowChinese(1, 48, "台灯：");
		OLED_ShowChinese(40, 48, (led_flag == 1) ? "开" : "关");
	}
	else if (mode == 3)
	{
		if (time_i == 0)
		{
			OLED_ShowChinese(4, 32, "↑");
			OLED_ClearArea(52, 32, 16, 16);
		}
		else if (time_i == 1)
		{
			OLED_ShowChinese(28, 32, "↑");
			OLED_ClearArea(4, 32, 16, 16);
		}
		else
		{
			OLED_ShowChinese(52, 32, "↑");
			OLED_ClearArea(28, 32, 16, 16);
		}
		sprintf(s, "%02d:%02d:%02d", residue_time.hour, residue_time.minute, residue_time.sec);
		OLED_ShowString(1, 16, s, 8);
		OLED_ShowChinese(1, 48, "增氧：");
		OLED_ShowChinese(40, 48, (oxy_flag == 1) ? "开" : "关");
		OLED_ShowChinese(60, 48, "喂食：");
		OLED_ShowChinese(100, 48, (motor_flag == 1) ? "开" : "关");
	}
}



uint8_t send_time;
void Data_Send(void)
{
	if (send_time < 20)
	{
		send_time++;
		return ;
	}
	send_time = 0;
	sprintf(s, "temp:%.1f,light:%d,water_level:%d,turbidity:%d,\r\n", temp_value, light, water_level, turbidity);
	ESP8266_SendData(s, strlen(s));
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  ESP8266_Init();
  Warm_Set_State(0);
	Water_Set_State(0);
	Buzzer_Set_State(0);
	LED_Set_State(0);
	OXY_Set_State(0);
	MOTOR_Start(0);
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		ESP8266_GetIPD(100);
		Key_Scan();
		ADC_GetValue();
		temp_value = DS18B20_GetTemp();
		Ctrl();
		Data_Send();
		Page();
		OLED_Update();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
