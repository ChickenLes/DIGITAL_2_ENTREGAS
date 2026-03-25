/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define size 128
#define PI 3.1415926
#define TIM_FREQ 84000000
#define PSC_DAC 1
#define ARR_PWM 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t Ysine[size];
uint8_t  rx_byte;
uint8_t  pending_action = 0;
uint8_t  state = 0;

char menu[] =
              "1. Cancion 1 \r\n"
              "2. Cancion 2  \r\n"
              "Seleccione una opcion: ";

// CANCION 1 (CHAT GPT)
int STmelody[] = { 523, 659, 784, 988, 1047, 988, 784, 659,
                   523, 659, 784, 988, 1047, 988, 784, 659,
                   523, 659, 784, 988, 1047, 988, 784, 659,
                   523, 659, 784, 988, 1047, 988, 784, 659,
                   523, 659, 784, 988, 1047, 988, 784, 659,
                   523, 659, 784, 988, 1047, 988, 784, 659 };

int STdurations[] = { 188, 188, 188, 188, 188, 188, 188, 188,
                      188, 188, 188, 188, 188, 188, 188, 188,
                      188, 188, 188, 188, 188, 188, 188, 188,
                      188, 188, 188, 188, 188, 188, 188, 188,
                      188, 188, 188, 188, 188, 188, 188, 188,
                      188, 188, 188, 188, 188, 188, 188, 188 };

// CANCION 2
int KidsMelody[] = { 784, 698, 523, 587, 784, 698, 523, 587, 784, 698, 523, 587,
                     622, 587, 466, 523, 622, 587, 466, 523, 622, 587, 466, 523,
                     784, 698, 523, 587, 784, 698, 523, 587, 784, 698, 523, 587,
                     622, 587, 466, 523, 622, 587, 466, 523, 622, 587, 466, 523 };

int KidsDurations[] = { 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
                        200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
                        200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
                        200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void Show_Menu(void);
void generarSin(void);
int calcularARR(int freq);
int presForFrequencyPWM(int frequency);
void noToneDAC(void);
void noTonePWM(void);
void playToneDAC(int *tone, int *duration, int *pause, int Nsize);
void playTonePWM(int *tone, int *duration, int *pause, int Nsize);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Show_Menu(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)menu, strlen(menu), 100);
}

// DAC
void generarSin(void) {
    for (int x = 0; x < size; x++) {
        Ysine[x] = ((sin(x * 2 * PI / size) + 1) * (4096 / 2));
    }
}

int calcularARR(int freq) {
    if (freq == 0) return 0;
    int TF = size * freq;
    return ((TIM_FREQ / ((PSC_DAC + 1) * TF)) - 1);
}

void noToneDAC(void) {
    TIM6->ARR = 0;
}

void playToneDAC(int *tone, int *duration, int *pause, int Nsize) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n(DAC)\r\n", 9, 100);
    state = 1;

    HAL_TIM_Base_Start(&htim6);
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)Ysine, size, DAC_ALIGN_12B_R);

    for (int i = 0; i < Nsize; i++) {
        int dur = duration[i];
        int pauseBetweenTones = 0;

        if (pause != NULL) {
            pauseBetweenTones = pause[i] - duration[i];
        }

        if (tone[i] == 0) {
            noToneDAC();
        } else {
            int valorARR = calcularARR(tone[i]);
            TIM6->ARR = valorARR;
        }

        HAL_Delay(dur);
        noToneDAC();

        if (pauseBetweenTones > 0) {
            HAL_Delay(pauseBetweenTones);
        } else {
            HAL_Delay(10);
        }
    }

    HAL_TIM_Base_Stop(&htim6);
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    state = 0;
    Show_Menu();
}

// PWM
int presForFrequencyPWM(int frequency) {
    if (frequency == 0) return 0;
    return ((TIM_FREQ / (ARR_PWM * frequency)) - 1);
}

void noTonePWM(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

void playTonePWM(int *tone, int *duration, int *pause, int Nsize) {
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n(PWM)\r\n", 9, 100);
    state = 2;

    __HAL_TIM_MOE_ENABLE(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

    for (int i = 0; i < Nsize; i++) {
        int dur = duration[i];
        int pauseBetweenTones = 0;

        if (pause != NULL) {
            pauseBetweenTones = pause[i] - duration[i];
        }

        if (tone[i] == 0) {
            noTonePWM();
        } else {
            int prescaler = presForFrequencyPWM(tone[i]);
            __HAL_TIM_SET_PRESCALER(&htim1, prescaler);
            htim1.Instance->EGR = TIM_EGR_UG;
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 50);
        }

        HAL_Delay(dur);
        noTonePWM();

        if (pauseBetweenTones > 0) {
            HAL_Delay(pauseBetweenTones);
        } else {
            HAL_Delay(10);
        }
    }

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_MOE_DISABLE(&htim1);
    state = 0;
    Show_Menu();
}

// UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (state == 0) {
            if (rx_byte == '1') pending_action = 1;
            else if (rx_byte == '2') pending_action = 2;
            else pending_action = 3;
        }
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
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
  MX_DAC_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  generarSin();

  Show_Menu();
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (pending_action == 1) {
          pending_action = 0;
          playToneDAC(STmelody, STdurations, NULL, (sizeof(STmelody)/sizeof(STmelody[0])));
      }
      else if (pending_action == 2) {
          pending_action = 0;
          playTonePWM(KidsMelody, KidsDurations, NULL, (sizeof(KidsMelody)/sizeof(KidsMelody[0])));
      }
      else if (pending_action == 3) {
          pending_action = 0;
          HAL_UART_Transmit(&huart2, (uint8_t*)"\r\nOpcion Invalida\r\n", 19, 100);
          Show_Menu();
      }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

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
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = ARR_PWM-1; // Usamos el macro ARR_PWM (100)
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
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = PSC_DAC;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 100-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  huart2.Init.BaudRate = 115200;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
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
