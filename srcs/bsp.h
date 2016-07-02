#ifndef		__BSP_H__
#define		__BSP_H__

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include 	<stm32f10x.h>
#include	"uart.h"
#include	"flash.h"
#include	"mg301.h"


#define		SYSCLK								72000000L

#define		PARAM_SAVE_ADDR				0x0800F000
#define		UID_SAVE_ADDR					0x0800F800

#define		USART1_BUF_MAX				512
#define		USART2_BUF_MAX				256
#define		UART4_BUF_MAX					256

#define		RTU_MSG_SIZE				1024

//

#define		IWDG_REFRESH()			IWDG->KR = 0xAAAA

//public function prototype
void board_init(void);
void sleep(uint16_t sec);
void delay(uint16_t ms);
void gpio_init(void);
void iwdg_init(void);
void regular_timer_init(uint32_t ms);
void tim3_handle(void);
uint8_t is_timer_uif(void);

#endif		//__BSP_H__
