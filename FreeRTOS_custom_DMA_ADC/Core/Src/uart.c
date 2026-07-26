#include "uart.h"
#include "cmsis_os.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include "adc.h"

UART_HandleTypeDef huart2;
volatile uint32_t adc_buffer_count = 0;

void ADC_Task(void *pvParameters)
{
    while(1)
    {
        xSemaphoreTake(dmaSemaphore,portMAX_DELAY);
        CDC_Transmit_FS((uint8_t *)readyBuffer,ADC_BUFFER_SIZE * sizeof(uint16_t));
        adc_buffer_count++;
    }
}

void MX_USART2_UART_TX_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
  }

}

void MX_USART2_UART_RX_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
  }

}

