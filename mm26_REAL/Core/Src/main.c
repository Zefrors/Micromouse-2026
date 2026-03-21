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
#include "custom_motion_sensors.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "itoa.h"
#include "ftoa.h"
#include "motors.h"
#include "delay.h"
#include "irs.h"
#include "motion_gc.h"
#include "motion_fx.h"
#include "motion_ac.h"
#include "solver.h"
#include "systick.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MFX_STR_LENG 35
#define STATE_SIZE (size_t)(2450)
#define ENABLE_6X 1
#define SAMPLE_FREQUENCY 416.0f
#define ACCEL_SENSITIVITY 0.488f
#define GYRO_SENSITIVITY 70.0f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
CRC_HandleTypeDef hcrc;
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CRC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
CUSTOM_MOTION_SENSOR_Axes_t gyr_value;
CUSTOM_MOTION_SENSOR_Axes_t acc_value;
CUSTOM_MOTION_SENSOR_AxesRaw_t gyr_raw;
CUSTOM_MOTION_SENSOR_AxesRaw_t acc_raw;
int16_t left_counts = 0;
int16_t right_counts = 0;
char ret;
char salutations[] = "HI!";
char xline[] = "FRONT: ";
char yline[] = "LEFT: ";
char zline[] = "RIGHT: ";
char empty[] = "                ";
char buffer[20];
int32_t x_vals[1000];
uint16_t x_cnt;
int32_t x_avg;
int32_t x_sum;
double x_pos = 0;
uint16_t motor_right_cnt;
uint16_t IR_VALUE;
uint16_t debug;
// motion gc
MGC_knobs_t knobs;
MGC_output_t start_gyro_bias;
float sample_freq = SAMPLE_FREQUENCY;
volatile float gyro_cal_x, gyro_cal_y, gyro_cal_z;
MGC_input_t data_in;
MGC_output_t data_out;
int bias_update = 0;
// other stuff
int16_t error;
int16_t enc;
//motion fx
static uint8_t mfxstate[STATE_SIZE];
MFX_knobs_t iKnobs;
// motion ac
MAC_knobs_t acc_knobs;
MAC_output_t start_acc_bias;
volatile float acc_cal_x, acc_cal_y, acc_cal_z;
MAC_input_t acc_data_in;
MAC_output_t acc_data_out;
uint8_t acc_bias_update = 0;
uint32_t CurrentTime, LastTime;
unsigned int calData[50];
MFX_input_t mfx_data_in;
MFX_output_t mfx_data_out;
float *q; /* Quaternion pointer to either to Game Rotation or Rotation vector (4 length) */
int32_t ret1, ret2, ret3;
int calibrating = 1;
float straight_angle;
float str_sum;
int n_str;
int is_cal_str;
uint16_t ir_f, ir_l, ir_r;
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
  MX_CRC_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  CUSTOM_MOTION_SENSOR_Init(CUSTOM_ISM330DHCX_0, MOTION_GYRO | MOTION_ACCELERO);
  CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_ISM330DHCX_0, MOTION_GYRO | MOTION_ACCELERO, (float_t) 416);
  CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_ISM330DHCX_0, MOTION_ACCELERO, 16);
  CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_ISM330DHCX_0, MOTION_GYRO, 2000);
  CUSTOM_MOTION_SENSOR_Enable(CUSTOM_ISM330DHCX_0, MOTION_GYRO | MOTION_ACCELERO);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  // PWM SETUP
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  HAL_TIM_Base_Start_IT(&htim4);
  __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
  Delay_Init();
  // GYRO INIT
  /*
  MotionGC_Initialize(MGC_MCU_STM32, &sample_freq);
  MotionGC_GetKnobs(&knobs);
  knobs.AccThr = 0.008f;
  knobs.GyroThr = 0.15;
  MotionGC_SetKnobs(&knobs);
  start_gyro_bias.GyroBiasX = 0;
  start_gyro_bias.GyroBiasY = 0;
  start_gyro_bias.GyroBiasZ = 0;
  MotionGC_SetCalParams(&start_gyro_bias);
  //sample_freq = SAMPLE_FREQUENCY;
  MotionGC_SetFrequency(&sample_freq);
  */
  // motion fx
  if (STATE_SIZE < MotionFX_GetStateSize())
  {
	  Error_Handler();
  }
  MotionFX_initialize(mfxstate);
  MotionFX_getKnobs(mfxstate, &iKnobs);
  iKnobs.gbias_gyro_th_sc = 0.01;
  iKnobs.gbias_acc_th_sc = 0.005;
  iKnobs.LMode = 1;
  MotionFX_setKnobs(mfxstate, &iKnobs);
  MotionFX_enable_6X(mfxstate, MFX_ENGINE_DISABLE);
  MotionFX_enable_9X(mfxstate, MFX_ENGINE_DISABLE);
  // motionac
  /*
  __HAL_RCC_CRC_CLK_ENABLE();
  MotionAC_Initialize(1);
  MotionAC_GetKnobs(&acc_knobs);
  acc_knobs.Sample_ms = sample_freq;
  (void)MotionAC_SetKnobs(&acc_knobs);
  */
//ssd1306_DrawPixel(1, 1, White);
  while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){}
  if (ENABLE_6X == 1)
  {
	  MotionFX_enable_6X(mfxstate, MFX_ENGINE_ENABLE);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ssd1306_SetCursor(0,0);
  ret = ssd1306_WriteString(salutations, Font_7x10, White);
  ssd1306_UpdateScreen();
  HAL_Delay(20000);
  setMotorSpeed(right, 2500, back);
  setMotorSpeed(left, 2500, back);
  HAL_Delay(1000);
  setMotorSpeed(right, 0, back);
  setMotorSpeed(left, 0, back);
  is_cal_str = 1;
  HAL_Delay(100);
  straight_angle = str_sum / n_str;
  str_sum = 0;
  n_str = 0;
  is_cal_str = 0;
  calibrating = 0;
  HAL_Delay(2400);
  center(straight_angle);
  HAL_Delay(2500);
  straight_angle += 90;
  if (straight_angle > 360)
	  straight_angle -= 360;
  HAL_Delay(2500);
  moveOne(straight_angle);
  while (1)
  {
	  // SOLVER CODE BEGIN
	  switch(floodFill()){
		  case(IDLE): {break;}
		  case(FORWARD): {moveOne(straight_angle); break;}
		  case(LEFT): {turnLeft(straight_angle); straight_angle += 90; break;}
		  case(RIGHT): {turnRight(straight_angle); straight_angle -= 90; break;}
	  }
	  if (straight_angle > 360){
		  straight_angle -= 360;
	  }
	  else if (straight_angle < -360){
		  straight_angle += 360;
	  }
	  HAL_Delay(2000);
	  // SOLVER CODE END

	  //moveOne(straight_angle);
	  /*
	  ssd1306_SetCursor(0,0);
	  itoa((int) getMotorEnc(right), buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);
	  memset(buffer,0,sizeof(buffer));
	  ssd1306_SetCursor(0,10);
	  itoa((int) getMotorEnc(left), buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);
	  memset(buffer,0,sizeof(buffer));
	  ssd1306_UpdateScreen();
	  setMotorSpeed(right, 10000, forward);
	  */

	  /*
	  ssd1306_SetCursor(0,0);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,0);
	  itoa(IR_VALUE, buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);
	  IR_VALUE = analogRead(IR_FRONT);
	  ssd1306_UpdateScreen();
	  */

	//setMotorSpeed(left, 10000, back);


	  /*
	  for (int i = 0; i < 500; i++){
		  x_avg += x_vals[i];
	  }
	  */
	  /*
	  ir_f = analogRead(IR_FRONT);
	  ir_l = analogRead(IR_LEFT);
	  ir_r = analogRead(IR_RIGHT);

	  i++;
	  ssd1306_SetCursor(0,0);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,0);
	  ret = ssd1306_WriteString(xline, Font_7x10, White);
	  //ftoa((float) (mfx_data_out.rotation)[0], buffer, (int) 10);
	  //ftoa(iKnobs.gbias_gyro_th_sc, buffer, (int) 10);
	  itoa(ir_f, buffer, (int) 10);

	  ret = ssd1306_WriteString(buffer, Font_7x10, White);

	  ssd1306_SetCursor(0,10);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,10);
	  ret = ssd1306_WriteString(yline, Font_7x10, White);
	  //ftoa(error, buffer, (int) 10);
	  //itoa((getMotorEnc(right) + getMotorEnc(left))/2, buffer, (int) 10);
	  itoa(ir_l, buffer, (int) 10);

	  ret = ssd1306_WriteString(buffer, Font_7x10, White);

	  ssd1306_SetCursor(0,20);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,20);
	  ret = ssd1306_WriteString(zline, Font_7x10, White);
	  itoa(ir_r, buffer, (int) 10);
	  //ftoa(straight_angle, buffer, (int) 10);
	  //ftoa((mfx_data_out.rotation)[2], buffer, (int) 10);
	  //ftoa((float) gyr_raw.z  / 8650, buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);


	  ssd1306_UpdateScreen();
	  */
	  //HAL_Delay(2500);

	  /*
	  ssd1306_SetCursor(0,10);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,10);
	  ret = ssd1306_WriteString(yline, Font_7x10, White);
	  memset(buffer,0,sizeof(buffer));
	  itoa((int) gyr_value.y, buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);
	  ssd1306_SetCursor(0,20);
	  ret = ssd1306_WriteString(empty, Font_7x10, White);
	  ssd1306_SetCursor(0,20);
	  ret = ssd1306_WriteString(zline, Font_7x10, White);
	  memset(buffer,0,sizeof(buffer));
	  itoa((int) gyr_value.z, buffer, (int) 10);
	  ret = ssd1306_WriteString(buffer, Font_7x10, White);
	  ssd1306_UpdateScreen();
	  */
	  //HAL_Delay(10);
	  //setMotorSpeed(right, (uint16_t) 65535*.5, back);
	  //setMotorSpeed(left, (uint16_t) 65535*.5, back);

	  //setMotorSpeed(right, (uint16_t) 65535, back);
	  //setMotorSpeed(left, (uint16_t) 65535, back);

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_16;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

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
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
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

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 3199;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 3;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 49999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA2_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OLED_DC_Pin|OLED_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Gyro_CS_GPIO_Port, Gyro_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_DC_Pin OLED_RST_Pin */
  GPIO_InitStruct.Pin = OLED_DC_Pin|OLED_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Gyro_CS_Pin LD2_Pin */
  GPIO_InitStruct.Pin = Gyro_CS_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : OLED_CS_Pin */
  GPIO_InitStruct.Pin = OLED_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
ADC_HandleTypeDef* Get_HADC1_Ptr(void)
{
    return &hadc1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
	//debug = HAL_GetTick();
	if (htim->Instance == TIM4){
		updateGyro();
	}
	 __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
	//while (TIM4->SR  != 0x0);
}

void updateError(int16_t e){
	error = e;
}

void updateGyro(){
	float dT;

	CUSTOM_MOTION_SENSOR_GetAxesRaw(CUSTOM_ISM330DHCX_0, MOTION_ACCELERO, &acc_raw);

	CurrentTime = HAL_GetTick();
	dT = ((float) CurrentTime - (float) LastTime) / 1000;

	CUSTOM_MOTION_SENSOR_GetAxesRaw(CUSTOM_ISM330DHCX_0, MOTION_GYRO, &gyr_raw);

	mfx_data_in.acc[0] = (float) acc_raw.x * (ACCEL_SENSITIVITY / 1000);
	mfx_data_in.acc[1] = (float) acc_raw.y * (ACCEL_SENSITIVITY / 1000);
	mfx_data_in.acc[2] = (float) acc_raw.z * (ACCEL_SENSITIVITY / 1000);
	mfx_data_in.gyro[0] = ((float) gyr_raw.x) * (GYRO_SENSITIVITY / 1000);
	mfx_data_in.gyro[1] = (float) gyr_raw.y * (GYRO_SENSITIVITY / 1000);
	mfx_data_in.gyro[2] = (float) gyr_raw.z * (GYRO_SENSITIVITY / 1000);

	MotionFX_propagate(mfxstate, &mfx_data_out, &mfx_data_in, &dT);
	MotionFX_update(mfxstate, &mfx_data_out, &mfx_data_in, &dT, NULL);
	if (ENABLE_6X == 1)
	{
	q = mfx_data_out.rotation;
	}
	else
	{
	/* Rotation Vector in 9X */
	q = mfx_data_out.rotation;
	}
	LastTime = CurrentTime;

	if (is_cal_str){
		str_sum += (mfx_data_out.rotation)[0];
		n_str++;
	}
	if (!calibrating)
		pid((mfx_data_out.rotation)[0]);

}
char MotionAC_LoadCalFromNVM (unsigned short intdataSize, unsigned int *data){
	return 0;
}
char MotionAC_SaveCalInNVM (unsigned short intdataSize, unsigned int *data){
	return 0;
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
