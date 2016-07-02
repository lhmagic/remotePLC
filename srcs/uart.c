#include	"uart.h"

volatile static char usart1_buf[USART1_BUF_MAX];
volatile static uint16_t usart1_rx_cnt;	
volatile static uint8_t usart1_rx_done;

volatile static char usart2_buf[USART2_BUF_MAX];
volatile static uint16_t usart2_rx_cnt;	
volatile static uint8_t usart2_rx_done;

//connect to RS485
void usart1_init(uint32_t baudrate) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	GPIOA->CRH &= ~0xFF0;
	GPIOA->CRH |= 0x490;
	
	USART1->CR1 |= USART_CR1_UE;
	USART1->BRR = SYSCLK/baudrate;
	USART1->CR2 |= USART_CR2_STOP_1;
	USART1->CR1 |= (USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);
}

void usart1_rx_handle(void) {
	
	if(USART1->SR & USART_SR_RXNE) {
		char ch = USART1->DR;
		usart_rx_to_on();
		
		usart1_buf[usart1_rx_cnt++] = USART1->DR;
		usart1_rx_cnt %= USART1_BUF_MAX;
	}
}

void usart_rx_to_on(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	TIM5->PSC = (uint16_t)(SYSCLK/100-1);
	TIM5->ARR = 10;
	
	TIM5->CNT = 10;
	TIM5->CR1 |= TIM_CR1_OPM | TIM_CR1_URS;
	TIM5->SR &= ~TIM_SR_UIF;
	TIM5->CR1 |= TIM_CR1_CEN;
	TIM5->DIER |= TIM_DIER_UIE;
	TIM5->SR &= ~TIM_SR_UIF;
	
	NVIC_ClearPendingIRQ(TIM5_IRQn);
	NVIC_SetPriority(TIM5_IRQn, 1);
	NVIC_EnableIRQ(TIM5_IRQn);	
}

void usart_rx_to_off(void) {
	TIM5->SR &= ~TIM_SR_UIF;
	TIM5->CR1 &= ~TIM_CR1_CEN;
	
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;
}

void tim5_handle(void) {
	usart_rx_to_off();
	usart1_rx_done = 1;
}

void xputc(const char ch) {
	while(!(USART1->SR & USART_SR_TXE));
	USART1->DR = ch;
}

void xputs(const char *msg) {
	while(*msg) {
		while(!(USART1->SR & USART_SR_TXE));
		USART1->DR = *msg++;
	}	
}

uint8_t is_usart1_rx_done(void) {
	if(usart1_rx_done == 1) {
		usart1_rx_done = 0;
		return 1;
	}
	return 0;
}

char *get_usart1_buf(void) {
char *p;
	p = (char *)usart1_buf;
	return p;
}

uint16_t get_usart1_rx_cnt(void) {
	return usart1_rx_cnt;
}

void usart1_buf_clr(void) {
uint16_t i;
	for(i=0; i<USART1_BUF_MAX; i++) {
		usart1_buf[i] = 0;
	}
	usart1_rx_cnt = 0;
	usart1_rx_done = 0;
}

//connect to MG301
void usart2_init(uint32_t baudrate) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	GPIOA->CRL &= ~0xFF00;
	GPIOA->CRL |= 0x4900;
	
	USART2->CR1 |= USART_CR1_UE;	
	USART2->BRR = SYSCLK/baudrate/2;
	USART2->CR1 |= USART_CR1_TE;
	USART2->CR1 |= (USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
	NVIC_SetPriority(USART2_IRQn, 1);
	NVIC_EnableIRQ(USART2_IRQn);
}

void usart2_rx_handle(void) {
	
	if(USART2->SR & USART_SR_RXNE) {
		char ch = USART2->DR;
		usart2_buf[usart2_rx_cnt++] = USART2->DR;
		usart2_rx_cnt %= USART2_BUF_MAX;
	}
}

void yputc(const char ch) {
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = ch;
}

void yputs(const char *msg) {
	while(*msg) {
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *msg++;
	}
}

//uint8_t is_usart2_rx_done(void) {
//	if(usart2_rx_done == 1) {
//		usart2_rx_done = 0;
//		return 1;
//	}
//	return 0;
//}

char *get_usart2_buf(void) {
char *p;
	p = (char *)usart2_buf;
	return p;
}

uint16_t get_usart2_rx_cnt(void) {
	return usart2_rx_cnt;
}

void usart2_buf_clr(void) {
uint16_t i;
	for(i=0; i<USART2_BUF_MAX; i++) {
		usart2_buf[i] = 0;
	}
	usart2_rx_cnt = 0;
	usart2_rx_done = 0;
}
