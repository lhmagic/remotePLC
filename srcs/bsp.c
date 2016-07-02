#include	"bsp.h"

void board_init(void) {
	iwdg_init();
	gpio_init();
	regular_timer_init(1000);
	usart1_init(115200);
	usart2_init(115200);
	mg_init();
}

void sleep(uint16_t sec) {
uint16_t i;
	for(i=0; i<sec; i++) {
		delay(1000);
		IWDG_REFRESH();
	}
}

void regular_timer_init(uint32_t ms) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = (uint16_t)(SYSCLK/100-1);
	TIM3->ARR = ms;
	
	TIM3->CNT = 1;
	TIM3->CR1 |= TIM_CR1_URS;
	TIM3->SR &= ~TIM_SR_UIF;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->SR &= ~TIM_SR_UIF;
	
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 1);
	NVIC_EnableIRQ(TIM3_IRQn);	
}

static uint8_t timer_uif;
void tim3_handle(void) {
	if(TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~TIM_SR_UIF;
		timer_uif = 1;
	}	
}

uint8_t is_timer_uif(void) {
uint8_t ret=0;
	if(timer_uif == 1) {
		timer_uif = 0;
		ret = 1;
	} 
	return ret;
}

void delay(uint16_t ms) {
	if(ms <= 0)
		return;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->PSC = (uint16_t)(SYSCLK/100-1);
	TIM4->ARR = ms;
	
	TIM4->CNT = 1;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM4->SR &= ~TIM_SR_UIF;
	while((TIM4->SR & TIM_SR_UIF) != TIM_SR_UIF);
	TIM4->CR1 &= ~TIM_CR1_CEN;
	
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;
}

void iwdg_init(void) {
	IWDG->KR = 0xCCCC; 
	IWDG->KR = 0x5555;
	IWDG->PR = IWDG_PR_PR; 
	IWDG->RLR = (40000/256)*2; 
	while (IWDG->SR) ;
	IWDG->KR = 0xAAAA; 
}

void gpio_init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	
	
	GPIOA->CRL &= ~0xF0;
	GPIOA->CRL |= 0x10;	
	GPIOA->ODR |= GPIO_ODR_ODR1;
	
	GPIOB->CRL &= ~0x0FFFF00F;
	GPIOB->CRL |= 0x01111001;
	
	GPIOC->CRH &= ~0x000F0000;
	GPIOC->CRH |= 0x00010000;
	
	GPIOD->CRL &= ~0x0F00;
	GPIOD->CRL |= 0x0100;

	GPIOA->CRH &= ~0x0F;
	GPIOA->CRH |= 0x08;
	GPIOA->ODR |= 0x100;
	
	GPIOB->CRH &= ~0xFFF00000;
	GPIOB->CRH |= 0x88800000;
	GPIOB->ODR |= 0xE000;
	
	GPIOC->CRL &= ~0xFF000000;
	GPIOC->CRH &= ~0x000000FF;
	GPIOC->CRL |= 0x88000000;
	GPIOC->CRH |= 0x00000088;
	GPIOC->ODR |= 0x3C0;
}
