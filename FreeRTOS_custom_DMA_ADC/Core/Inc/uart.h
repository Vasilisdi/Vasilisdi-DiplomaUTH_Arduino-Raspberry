#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "cmsis_os.h"

void ADC_Task(void *pvParameters);

void MX_USART2_UART_TX_Init(void);
void MX_USART2_UART_RX_Init(void);

#endif
