#include "stm32f4xx_hal.h"
#include "adc.h"

volatile uint32_t dma_transfer_count = 0; //counter to measure the real DMA transfers in a minute
volatile uint32_t dma_measurement_start = 0;
volatile uint8_t dma_measurement_active = 0;
volatile uint32_t semaphore_overwrite_count = 0;
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint16_t adc_buffer_spare[ADC_BUFFER_SIZE];

SemaphoreHandle_t dmaSemaphore = NULL;
volatile uint16_t *readyBuffer = NULL;

void adc_init(void)
{
    RCC->AHB1ENR |= (1U<<0); //enables the clock for GPIOA on the AHB1 bus.

    RCC->APB2ENR |= (1U<<8); // ADC1 clock enable

    RCC->AHB1ENR |= (1U<<22); //enables clock of dma2 at AHB1 bus

    GPIOA->MODER |= (3U<<0); //PA0 3=11 analog mode
    GPIOA->MODER |= (3U<<2); //PA1 3=11 analog mode
    GPIOA->MODER |= (3U<<4); //PA2 0x110000 analog mode for 3rd channel (3U=0b11) - PA2 -> bits 5:4 = 11 (analog)

    ADC1->CR2 = 0;  //makes zero the whole CR2 of ADC1 prior to configuration

    ADC1->SMPR2 = (7U << 0) | (7U << 3) | (7U << 6); //PA0 - CH0 bits 0,1,2 7=0b111 &CH1 3,4,5 & CH2 6,7,8 -> sample time 480 ADC cycles

    ADC1->CR2 |= (1U<<8); //enables dma for adc1

    ADC1->CR2 |= (1U<<9); //dma continuous requests - enables DDS bit at CR2

    ADC1->CR2 |= (1U<<1); // Continuous conversion

    ADC1->SQR1 = (2U<<20); //definition of the number of conversions L+1

    ADC1->SQR3 =(0U<<0) | (1U<<5) | (2U<<10); //conversion sequence CH0 → CH1 → CH2 → CH0 → CH1 → CH2...
    //rank1 0:4 , rank2 5:9 , rank3 10:14

    /*
       DMA2 Stream0 Channel0
    */
    DMA2_Stream0->CR &= ~(1U);    // disable

    while(DMA2_Stream0->CR & 1U);

    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR; //definition of data source

    DMA2_Stream0->M0AR =(uint32_t)adc_buffer;
    DMA2_Stream0->M1AR =(uint32_t)adc_buffer_spare; //the dma fills the second buffer while cpu works on the later

    DMA2_Stream0->NDTR = ADC_BUFFER_SIZE; //definition of the data target

    DMA2_Stream0->CR =0; // Reset of the control register

    DMA2_Stream0->CR &= ~(7U<<25); //clean 25:27 bits [000], in order for stream to use channel 0

    DMA2_Stream0->CR &= ~(3U<<6); //clean - zero 6,7 bits

    DMA2_Stream0->CR |= (1U<<8); //arrange circular mode between buffers

    DMA2_Stream0->CR |= (1U<<18); // usage of M0AR και M1AR buffer and its spare

    DMA2_Stream0->CR |= (1U<<10);  //memory increment - increase after each sample

    DMA2_Stream0->CR |= (1U<<11); //peripheral size bits 11,12 if CR if PSIZE 01->16bit otherwise 00->8bit and 10 -> 32bit

    DMA2_Stream0->CR |= (1U<<13); //memory size 16bit , for 13,14 01 -> 16bit

    DMA2_Stream0->CR |= (1U << 4); //TCIE (Transfer Complete Interrupt Enable) interrupt on buffer needed for double buffer

    //Now the CPU will receive an interrupt every time: DMA filled 3000 values -> ISR energized
    NVIC_SetPriority(DMA2_Stream0_IRQn,5); //priority 5 of interrupt of dma2 stream0
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);  //enable the interrupt at NVIC

    DMA2_Stream0->CR |=1; //dma stream starts, NDTR starts countdown dma awaits adc to send data

    ADC1->CR2 |=1; //adc enabled but conversion do not start just yet, it is stand by mode

    ADC1->CR2 |= (1U<<30); //starts conversions + DNA+DDS every CH0-CH1-CH2-CH0... sequence goes to DMA ->buffer->interrup->buffer spare->interrupt (NDTR reduces in the meanwhile -> DMA interrupt)

    dma_measurement_start = HAL_GetTick();
    dma_measurement_active = 1;

}



void DMA2_Stream0_IRQHandler(void)
{
    if(DMA2->LISR & (1U<<5))
    {
        DMA2->LIFCR |= (1U<<5);

        if(dma_measurement_active)
        {
            if((HAL_GetTick() - dma_measurement_start) < 60000)
            {
                dma_transfer_count++;       //counter of the completed dma transfers
            }
            else
            {
                dma_measurement_active = 0;
            }
        }

        if (DMA2_Stream0->CR & DMA_SxCR_CT)
        {
            readyBuffer = (uint16_t *)adc_buffer;
        }
        else
        {
            readyBuffer = (uint16_t *)adc_buffer_spare;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t result = xSemaphoreGiveFromISR(dmaSemaphore,&xHigherPriorityTaskWoken);
        if(result == pdFALSE)
        {
            semaphore_overwrite_count++;
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
