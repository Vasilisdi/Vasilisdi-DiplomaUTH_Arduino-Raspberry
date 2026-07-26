#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "cmsis_os.h"
#include "task.h"

#define ADC_BUFFER_SIZE 3000

extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint16_t adc_buffer_spare[ADC_BUFFER_SIZE];
extern volatile uint32_t dma_transfer_count;

extern SemaphoreHandle_t dmaSemaphore;
extern volatile uint16_t *readyBuffer;

void adc_init(void);
void DMA2_Stream0_IRQHandler(void);

#endif
