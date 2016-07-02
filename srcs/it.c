#include	"bsp.h"

void EXTI0_1_IRQHandler(void) {

}

void USART1_IRQHandler(void) {
	usart1_rx_handle();
}

void USART2_IRQHandler(void) {
	usart2_rx_handle();
}

void TIM3_IRQHandler(void) {
	tim3_handle();
}

void TIM5_IRQHandler(void) {
	tim5_handle();
}
