#include "stm32f4xx_hal.h"


void pa0_interrupt_init(void)
{
	RCC->AHB1ENR |= (1U<<0);       // GPIOA clock
	RCC->APB2ENR |= (1U<<14);      // SYSCFG clock

	GPIOA->MODER &= ~(3U<<0);      // PA0 input

	SYSCFG->EXTICR[0] &= ~(0xF);   // PA0 source

	EXTI->IMR |= (1U<<0);          // enable EXTI0

	EXTI->FTSR |= (1U<<0);         // falling edge

	NVIC_SetPriority(EXTI0_IRQn,6);
	NVIC_EnableIRQ(EXTI0_IRQn);
}



void gpio_init(void)
{
	RCC->AHB1ENR |= (1U<<0);

	GPIOA->MODER &= ~(3U<<0);
}


uint8_t read_digital_sensor(void)
{
	if(GPIOA->IDR & 1)
		return 1;

	return 0;
}
