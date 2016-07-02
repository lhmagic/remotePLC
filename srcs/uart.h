#ifndef		__UART_H__
#define		__UART_H__

#include	"bsp.h"

//public function prototype
void usart1_init(uint32_t baudrate);
void usart1_rx_handle(void);
uint8_t is_usart1_rx_done(void);
char *get_usart1_buf(void);
uint16_t get_usart1_rx_cnt(void);
void usart1_buf_clr(void);
void xputc(const char ch);
void xputs(const char *msg);

void usart2_init(uint32_t baudrate);
void usart2_rx_handle(void);
//uint8_t is_usart2_rx_done(void);
char *get_usart2_buf(void);
uint16_t get_usart2_rx_cnt(void);
void usart2_buf_clr(void);
void yputc(const char ch);
void yputs(const char *msg);

void usart_rx_to_on(void) ;
void usart_rx_to_off(void);
void tim5_handle(void);


//private function prototype


#endif		//__UART_H__
