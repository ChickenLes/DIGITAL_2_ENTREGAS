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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIM_FREQ 84000000
#define PSC_DAC 1
#define size 128
#define PI 3.1415926535

// ==========================================
// DICCIONARIO DE NOTAS (Frecuencias en Hz)
// ==========================================

// --- BAJOS (Octavas 2 y 3) - Para ritmos pesados y marchas ---
#define C2  65
#define Db2 69
#define D2  73
#define Eb2 78
#define E2  82
#define F2  87
#define Gb2 92
#define G2  98
#define Ab2 104
#define A2  110
#define Bb2 117
#define B2  123

#define C3  131
#define Db3 139
#define D3  147
#define Eb3 156
#define E3  165
#define F3  175
#define Gb3 185
#define G3  196
#define Ab3 208
#define A3  220
#define Bb3 233
#define B3  247

// --- MEDIOS (Octavas 4, 5 y 6) - Melodías principales ---
#define C4  262
#define Db4 277
#define D4  294
#define Eb4 311
#define E4  330
#define F4  349
#define Gb4 370
#define G4  392
#define Ab4 415
#define A4  440
#define Bb4 466
#define B4  494

#define C5  523
#define Db5 554
#define D5  587
#define Eb5 622
#define E5  659
#define F5  698
#define Gb5 740
#define G5  784
#define Ab5 831
#define A5  880
#define Bb5 932
#define B5  988

#define C6  1047
#define Db6 1109
#define D6  1175
#define Eb6 1245
#define E6  1319
#define F6  1397
#define Gb6 1480
#define G6  1568
#define Ab6 1661
#define A6  1760
#define Bb6 1865
#define B6  1976

// --- AGUDOS Y SFX (Octava 7) - Monedas, saltos, láseres ---
#define C7  2093
#define Db7 2217
#define D7  2349
#define Eb7 2489
#define E7  2637
#define F7  2794
#define Gb7 2960
#define G7  3136
#define Ab7 3322
#define A7  3520
#define Bb7 3729
#define B7  3951

// Direccion I2C del ESP32 (7-bit, shifted left for HAL)
#define ESP32_I2C_ADDR  (0x10 << 1)

// Comandos recibidos del ESP32
#define CMD_RIGHT   'R'
#define CMD_LEFT    'L'
#define CMD_UP      'U'
#define CMD_DOWN    'D'
#define CMD_A       'A'
#define CMD_B       'B'
#define CMD_PAUSE   'T'
#define CMD_PLAY    'P'
#define CMD_DEATH   'M'
#define CMD_WIN     'W'
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t Ysine[size];
uint8_t  receivedRX = 0;
volatile uint8_t flagChange = 0;

// --- I2C interrupt variables ---
uint8_t  i2cRxBuf = 0;
volatile uint8_t flagI2C = 0;
volatile uint8_t i2cReady = 1;

// --- Estado de canciones ---
uint8_t currentSong = 0;
uint8_t isPaused = 0;

// =====================================================================
// EFECTO: R y L - Paso corto (tap tap)
// =====================================================================
int sfx_walk[] = {
    E4, 0, G4, 0, E4, 0
};
int dur_walk[] = {
    40, 20, 40, 20, 40, 30
};

// =====================================================================
// EFECTO: U (Arriba) - Blip ultra corto
// =====================================================================
int sfx_up[] = {
    C5, E5
};
int dur_up[] = {
    25, 35
};

// =====================================================================
// EFECTO: D (Abajo) - Boop descendente suave
// =====================================================================
int sfx_down[] = {
    E5, C5, A4
};
int dur_down[] = {
    35, 35, 50
};

// =====================================================================
// EFECTO: A - Golpe seco (hit)
// =====================================================================
int sfx_a[] = {
    C6
};
int dur_a[] = {
    25
};

// =====================================================================
// EFECTO: B - Salto tipo Mario (sweep rapido hacia arriba)
// =====================================================================
int sfx_b[] = {
    C4, E4, G4, C5, E5
};
int dur_b[] = {
    20, 20, 20, 20, 30
};

// =====================================================================
// CANCION T (Pausa) - Tema chill de espera estilo 8-bit
// =====================================================================
int song_pause[] = {
		// El "Riff" de bajo inicial (muy rápido)
		    G4, G4, Bb4, G4, C5, G4, Bb4, G4,
		    G4, G4, Bb4, G4, C5, G4, Bb4, G4,

		    // Melodía de entrada (Heroica)
		    D5, G4, Bb4, C5, D5, G4, Bb4, C5,
		    D5, G4, Bb4, C5, D5, Eb5, D5, C5,

		    // Subida de tensión
		    G5, 0, G5, 0, F5, 0, F5, 0,
		    Eb5, 0, Eb5, 0, D5, 0, D5, 0,

		    // El Clímax (Spear of Justice acelerado)
		    G5, F5, Eb5, D5, C5, Bb4, A4, G4,
		    D5, G5, F5, G5, A5, Bb5, C6, D6
};
int dur_pause[] = {
		// Riff inicial (80ms por nota para que se sienta el galope)
		    80, 80, 80, 80, 80, 80, 80, 80,
		    80, 80, 80, 80, 80, 80, 80, 80,

		    // Melodía (Más rápida que la anterior)
		    120, 120, 120, 120, 120, 120, 120, 120,
		    120, 120, 120, 120, 120, 120, 120, 120,

		    // Tensión (Notas cortadas / Staccato)
		    100, 50, 100, 50, 100, 50, 100, 50,
		    100, 50, 100, 50, 100, 50, 100, 50,

		    // Clímax
		    100, 100, 100, 100, 100, 100, 100, 100,
		    100, 100, 100, 100, 100, 100, 100, 400
};

// =====================================================================
// CANCION P (Play) - Tema energico de accion 8-bit
// =====================================================================
int song_play[] = {
    E5, E5, 0, E5, 0, C5, E5, 0,
    G5, 0, G4, 0,
    C5, 0, G4, 0, E4, 0,
    A4, B4, Bb4, A4, 0,
    G4, E5, G5, A5, 0,
    F5, G5, 0, E5, 0,
    C5, D5, B4, 0
};
int dur_play[] = {
    100, 100, 50, 100, 50, 100, 100, 100,
    200, 100, 200, 100,
    150, 50, 150, 50, 150, 50,
    100, 100, 100, 150, 50,
    100, 100, 100, 200, 50,
    100, 100, 50, 150, 50,
    100, 100, 200, 200
};

// =====================================================================
// M (Muerte) - Game Over Arcade (extraido de freesound MP3)
// 35 notas, ~1s - bajada cromatica dramatica
// =====================================================================
int tone_death[] = {
		// Paso lento inicial (Pa - Pa - Pa, pira-Pa)
		    C4, 0, C4, 0, C4, Eb4, D4, 0,
		    // Segundo paso, bajando de tono
		    D4, 0, C4, G4, 0,
		    // Caída dramática final hacia la tumba
		    Ab4, G4, F4, Eb4, D4, C4
};
int dur_death[] = {
		// Tiempos largos con silencios pesados
		    500, 100, 500, 100, 350, 150, 800, 100,
		    500, 100, 300, 800, 100,
		    // La caída lenta
		    400, 400, 400, 400, 400, 1200
};

// =====================================================================
// W (Win) - 8-bit Victory Theme (extraido de moodmode MP3)
// 254 notas, ~12s - melodia de victoria
// =====================================================================
int tone_win[] = {
		// Compás 2
		    E5, E5, D5, E5,
		    // Compás 3
		    Gb5, E5, Eb5, E5,
		    // Compás 4
		    D5, D5, Db5, D5,
		    // Compás 5
		    E5, D5, Db5, B4,
		    // Compás 6
		    C5, C5, B4, A4,
		    // Compás 7
		    Ab4, A4, B4
};
int dur_win[] = {
		160, 80, 160, 80, // Ritmo: Ta - ta, Ta - ta
		    160, 80, 160, 80,
		    160, 80, 160, 80,
		    160, 80, 160, 80,
		    160, 80, 160, 80,
		    160, 80, 240      // Cierre del intro
};

char mensaje[] = "\r\n--- CONSOLA DK (DAC + I2C IT) ---\r\nComandos: R L U D A B T P M W\r\n> ";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void generarSin(void);
int  calcularARR(int freq);
void playTone_DAC1(int *tone, int *duration, int Nsize);
void sendMenu(void);
void sendUART(char *str);
void processCommand(uint8_t cmd);
void startI2C_Receive_IT(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void generarSin(void) {
    for (int x = 0; x < size; x++) {
        Ysine[x] = (uint32_t)(2048.0 + (sin(x * 2.0 * PI / size) * 800.0));
    }
}

int calcularARR(int freq) {
    if (freq <= 0) return 65535;
    return ((TIM_FREQ / ((PSC_DAC + 1) * size * freq)) - 1);
}

void sendUART(char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void sendMenu(void) {
    HAL_UART_Transmit(&huart2, (uint8_t*)mensaje, strlen(mensaje), HAL_MAX_DELAY);
    HAL_Delay(100);
}

void playTone_DAC1(int *tone, int *duration, int Nsize) {
    HAL_TIM_Base_Start(&htim6);

    for (int i = 0; i < Nsize; i++) {
        if (tone[i] == 0) {
            HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
        } else {
            htim6.Instance->ARR = calcularARR(tone[i]);
            HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)Ysine, size, DAC_ALIGN_12B_R);
        }

        HAL_Delay(duration[i]);

        HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
        HAL_Delay(15);
    }

    HAL_TIM_Base_Stop(&htim6);
}

// =====================================================================
// Lanzar recepcion I2C por interrupcion
// =====================================================================
void startI2C_Receive_IT(void) {
	i2cReady = 0;

	    HAL_I2C_Slave_Receive_IT(&hi2c1, &i2cRxBuf, 1);
}

// =====================================================================
// Procesar comando recibido (UART o I2C)
// =====================================================================
void processCommand(uint8_t cmd) {
    char buf[60];

    switch (cmd) {

    case CMD_RIGHT:
    case 'r':
        sendUART("\r\n>> [R] Derecha\r\n");
        playTone_DAC1(sfx_walk, dur_walk, sizeof(sfx_walk)/sizeof(sfx_walk[0]));
        break;

    case CMD_LEFT:
    case 'l':
        sendUART("\r\n>> [L] Izquierda\r\n");
        playTone_DAC1(sfx_walk, dur_walk, sizeof(sfx_walk)/sizeof(sfx_walk[0]));
        break;

    case CMD_UP:
    case 'u':
        sendUART("\r\n>> [U] Arriba\r\n");
        playTone_DAC1(sfx_up, dur_up, sizeof(sfx_up)/sizeof(sfx_up[0]));
        break;

    case CMD_DOWN:
    case 'd':
        sendUART("\r\n>> [D] Abajo\r\n");
        playTone_DAC1(sfx_down, dur_down, sizeof(sfx_down)/sizeof(sfx_down[0]));
        break;

    case CMD_A:
    case 'a':
        sendUART("\r\n>> [A] Golpe\r\n");
        playTone_DAC1(sfx_a, dur_a, sizeof(sfx_a)/sizeof(sfx_a[0]));
        break;

    case CMD_B:
    case 'b':
        sendUART("\r\n>> [B] Salto\r\n");
        playTone_DAC1(sfx_b, dur_b, sizeof(sfx_b)/sizeof(sfx_b[0]));
        break;

    case CMD_PAUSE:
    case 't':
        isPaused = 1;
        sendUART("\r\n>> [T] Pausa\r\n");
        playTone_DAC1(song_pause, dur_pause, sizeof(song_pause)/sizeof(song_pause[0]));
        break;

    case CMD_PLAY:
    case 'p':
        isPaused = 0;
        if (currentSong == 0) {
            sendUART("\r\n>> [P] Play - Tema accion\r\n");
            playTone_DAC1(song_play, dur_play, sizeof(song_play)/sizeof(song_play[0]));
            currentSong = 1;
        } else {
            sendUART("\r\n>> [P] Play - Tema chill\r\n");
            playTone_DAC1(song_pause, dur_pause, sizeof(song_pause)/sizeof(song_pause[0]));
            currentSong = 0;
        }
        break;

    case CMD_DEATH:
    case 'm':



        {  // <-- Agrega esta llave
        	 sendUART("\r\n>> [M] GAME OVER\r\n");

                // Calculamos cuántas notas tiene la canción
                int size_death = sizeof(tone_death) / sizeof((tone_death)[0]);

                // Creamos un arreglo temporal para las duraciones nuevas
                int dur_lenta[size_death];

                for (int i = 0; i < size_death; i++) {
                    // MULTIPLICADOR DE VELOCIDAD:
                    // Pon * 2, * 3 o * 4 hasta encontrar la velocidad perfecta.
                    dur_lenta[i] = dur_death[i] * 1;
                }

                // Reproducimos usando el arreglo modificado
                playTone_DAC1(tone_death, dur_lenta, size_death);
                break;
            }  // <-- Y cierra la llave aquí
    case CMD_WIN:
    case 'w':
    {  // <-- Agrega esta llave
            sendUART("\r\n>> [W] VICTORY!\r\n");

            // Calculamos cuántas notas tiene la canción
            int size_win = sizeof(tone_win) / sizeof(tone_win[0]);

            // Creamos un arreglo temporal para las duraciones nuevas
            int dur_lenta[size_win];

            for (int i = 0; i < size_win; i++) {
                // MULTIPLICADOR DE VELOCIDAD:
                // Pon * 2, * 3 o * 4 hasta encontrar la velocidad perfecta.
                dur_lenta[i] = dur_win[i] * 1;
            }

            // Reproducimos usando el arreglo modificado
            playTone_DAC1(tone_win, dur_lenta, size_win);
            break;
        }  // <-- Y cierra la llave aquí

    default:
        sprintf(buf, "\r\n>> Comando no reconocido: 0x%02X ('%c')\r\n", cmd, cmd);
        sendUART(buf);
        break;
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  generarSin();





  // Arrancar UART RX por interrupcion
  HAL_UART_Receive_IT(&huart2, &receivedRX, 1);

  // Arrancar primera recepcion I2C por interrupcion
  startI2C_Receive_IT();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  sendMenu();
  while (1)
  {
      // --- Dato recibido por UART ---
      if (flagChange) {
          flagChange = 0;
          processCommand(receivedRX);
          sendMenu();
      }

      // --- Dato recibido por I2C (interrupcion) ---
      if (flagI2C) {
          flagI2C = 0;
          char buf[40];
          sprintf(buf, "\r\n[I2C] Recibido: '%c' (0x%02X)\r\n", i2cRxBuf, i2cRxBuf);
          sendUART(buf);
          processCommand(i2cRxBuf);
          sendMenu();

          // Re-armar la siguiente recepcion I2C
          startI2C_Receive_IT();
      }

      // Si la recepcion I2C se perdio (error/timeout), re-armar
      if (i2cReady) {
    	  HAL_Delay(5);
          startI2C_Receive_IT();
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;

  // ¡AQUÍ ESTABA EL ERROR 1!
  hi2c1.Init.OwnAddress1 = ESP32_I2C_ADDR;

  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100-1;
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
  htim6.Init.Prescaler = 1-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000-1;
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

// =====================================================================
// CALLBACK: UART RX completo
// =====================================================================
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        flagChange = 1;
        HAL_UART_Receive_IT(&huart2, &receivedRX, 1);
    }
}

// =====================================================================
// ¡AQUÍ ESTABA EL ERROR 2! Reemplazado por HAL_I2C_SlaveRxCpltCallback
// =====================================================================
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        if (i2cRxBuf != 0x00 && i2cRxBuf != 0xFF) {
            flagI2C = 1;
        } else {
            i2cReady = 1;
        }
    }
}

// =====================================================================
// CALLBACK: I2C Error (NACK, bus error, etc.)
// =====================================================================
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        i2cReady = 1;
    }
}

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
