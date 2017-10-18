/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "dma.h"
#include "lptim.h"
#include "usart.h"
#include "rtc.h"
#include "spi.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "mutex.h"
#include "gpio-int.h"
#include "platform/platform.h"
#include "radio/radio.h"
#include "radio/t1_c1_packet_decoder.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

enum {
	SPI_TRANSFER_WAIT,
	SPI_TRANSFER_COMPLETE,
	SPI_TRANSFER_ERROR
};

static volatile int spiTransferState = SPI_TRANSFER_WAIT;

#define SPI_RX_BUFFER_SIZE		64  							    // Define the payload size here
static volatile uint8_t spiRxBuffer1[SPI_RX_BUFFER_SIZE];			// RF buffer 1
static volatile uint8_t spiRxBuffer2[SPI_RX_BUFFER_SIZE];			// RF buffer 2

static volatile uint8_t *spiRxBuffer = spiRxBuffer1;
static volatile uint8_t *spiWorkBuffer = spiRxBuffer2;

static const uint32_t ACCESS_CODE = 0b0101010101010000111101u;
static const uint32_t ACCESS_CODE_BITMASK = 0x3FFFFFu;

extern SPI_HandleTypeDef hspi3; /* SPI where we collect data */
extern UART_HandleTypeDef huart4;

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  spiWorkBuffer = spiRxBuffer;
  spiTransferState = SPI_TRANSFER_COMPLETE;

  if (spiRxBuffer == spiRxBuffer1)
	  spiRxBuffer = spiRxBuffer2;
  else
	  spiRxBuffer = spiRxBuffer1;

  if(HAL_SPI_Receive_DMA(hspi, spiRxBuffer, SPI_RX_BUFFER_SIZE) != HAL_OK)
  {
	Error_Handler();
  }
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	(void)hspi;

	Error_Handler();
}

enum
{
	UART_DMA_TRANSFER_WAIT,
	UART_DMA_TRANSFER_COMPLETE,
	UART_DMA_TRANSFER_ERROR
};

volatile int uartDmaTransferStatus = UART_DMA_TRANSFER_COMPLETE;

volatile static uint8_t uart4RxBuffer[128];
volatile static size_t uart4RxBufferIdx;

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	(void)UartHandle;

  /* Set transmission flag: trasfer complete*/
	uartDmaTransferStatus = UART_DMA_TRANSFER_COMPLETE;
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
  uart4RxBufferIdx = (uart4RxBufferIdx + 4u) & (sizeof(uart4RxBuffer)-1);

  if (HAL_UART_Receive_DMA(UartHandle, &uart4RxBuffer[uart4RxBufferIdx], 4u)!= HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	(void)UartHandle;

    Error_Handler();
}

int __io_putbuf(char *buf, int len)
{
  while (uartDmaTransferStatus == UART_DMA_TRANSFER_WAIT);

  uartDmaTransferStatus = UART_DMA_TRANSFER_WAIT;

  if(HAL_UART_Transmit_DMA(&huart4, (uint8_t*)buf, len)!= HAL_OK)
  {
    Error_Handler();
  }

  return len;
}

int __io_putchar(int ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_RTC_Init();
  MX_UART4_Init();
  MX_LPTIM1_Init();
  MX_LPUART1_UART_Init();

  /* USER CODE BEGIN 2 */
  uint32_t syncPattern = 0;
  unsigned overflow = 0;
  unsigned crc_ok_of_total_packets = 0;
  unsigned total_packets = 0;


  tRadioDriver *Radio = RadioDriverInit();
  Radio->Init( );
  Radio->StartRx();

  if(HAL_SPI_Receive_DMA(&hspi3, spiRxBuffer, SPI_RX_BUFFER_SIZE) != HAL_OK)
  {
	Error_Handler();
  }

  if (HAL_UART_Receive_DMA(&huart4, &uart4RxBuffer[uart4RxBufferIdx], 4u)!= HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  while (spiTransferState == SPI_TRANSFER_WAIT);

	  spiTransferState = SPI_TRANSFER_WAIT;

	  // Process spiWorkBuffer
	  for (int i = 0; i < SPI_RX_BUFFER_SIZE; i++)
	  {
	    for (int j = 8; j--; )
		{
	      unsigned bit = (spiWorkBuffer[i] >> j) & 0x01;

	      syncPattern = (syncPattern << 1) | bit;

		  if ((syncPattern & ACCESS_CODE_BITMASK) == ACCESS_CODE)
		  {
		    bit |= PACKET_PREAMBLE_DETECTED_MASK;
		  }

		  const struct t1_c1_packet_decoder_work_t *p = t1_c1_packet_decoder(bit, 255);

		  if (p)
		  {
			    static char printbuf[3 + 2 + 2 + 9 + 290 * 2 + sizeof('\n') + sizeof('\0')];

		        if (p->crc_ok) ++crc_ok_of_total_packets;
		        ++total_packets;

				int off = sprintf(printbuf,
						"%s;%u;%u;%08X;", p->c1_packet ? "C1": "T1",
						p->crc_ok,
						p->err_3outof6^1,
						get_serial(p->packet));

				for (size_t l = 0; l < p->L; l++)
					off += sprintf(printbuf + off, "%02x", p->packet[l]);

				off += sprintf(printbuf + off, "\n");

		    printf(printbuf);
		  }
		}
	  }

	  if (spiTransferState == SPI_TRANSFER_COMPLETE)
	  {
		// The line below typically means that we had not enough time to process data in spiWorkBuffer.
	    //Error_Handler();
		  ++overflow;
	  }
  }
  
  return 0;  
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  printf("Error: file %s on line %d\r\n", file, line);
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  printf("Wrong parameters value: file %s on line %lu\r\n", file, line);
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
