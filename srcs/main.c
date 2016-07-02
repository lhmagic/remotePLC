#include	"bsp.h"

/*****************************************
	PC12		-->			OUT1
	PD2			-->			OUT2
	PB3			-->			OUT3
	PB4			-->			OUT4
	PB5			-->			OUT5
	PB6			-->			OUT6
	PB0			-->			OUT7
	
	PA8			-->			IN1
	PC9			-->			IN2
	PC8			-->			IN3
	PC7			-->			IN4
	PC6			-->			IN5
	PB15		-->			IN6
	PB14		-->			IN7
	PB13		-->			IN8
 *****************************************/

#define		PARAM_ADDR		0x0800F000

uint8_t get_input(void);
void set_output(uint8_t out);
uint8_t get_output(void);

struct{
	char server[64];
	char phone[16];
} remote_config;

int main(void) {
static uint8_t last_input;	
static char buf[16];
	
	board_init();
	memcpy(&remote_config, (uint8_t *)PARAM_ADDR, 80);
	set_output(0x00);
	
	while(1) {
		IWDG_REFRESH();
		//串口配置处理
		if(is_usart1_rx_done()) {
		char *p, cnt, cmd;
		uint8_t i, start=0, wating_end=0, parm=0;
			p = get_usart1_buf();
			cnt = get_usart1_rx_cnt();
			cmd = *p;
			
			switch(cmd) {
				case 'R':
					xputs("R\"");
					xputs(remote_config.server);
					xputs("\" \"");
					xputs(remote_config.phone);
					xputs("\"\r\n");
					break;
				case 'W':
					memset(&remote_config, 0, 80);
					for(i=0; i<cnt; i++) {
						if(p[i] == '"') {
							if(wating_end==0) {
								wating_end = 1;
								start = i+1;
							} else {
								wating_end = 0;
								if(parm == 0) {
									memcpy(remote_config.server, p+start, i-start);
									parm++;
								} else {
									memcpy(remote_config.phone, p+start, i-start);
									parm++;
								}
							}
						}
					}

					flash_write(PARAM_ADDR, (uint8_t *)&remote_config, 80);
					xputs("OK\r\n");
					net_close(0);
					break;
				default:
					break;
			}
			usart1_buf_clr();
		}
		//输入端口状态改变时主动上报
		if(last_input != get_input()) {
			last_input = get_input();
			buf[0] = 0x01;
			buf[1] = get_input();
			net_write(0, buf, 2);
		}
		//定时检查
		if(is_timer_uif()) {
			//如果连接断开，则自动重连
			if(!is_net_connected(0)) {
				set_profile(0, remote_config.server, "apn", "name", "passwd");
				if(net_open(0) != 0) {
	//				mg_soft_restart();
				}				
			}
			//处理接收到的网络数据
			if(net_read(0, buf, 16) != 0) {
				switch(buf[0]) {
					case 0x01:
						buf[1] = get_input();
						break;
					case 0x02:
						buf[1] = get_output();
						break;
					case 0x03:
						set_output(buf[1]);
						buf[1] = get_output();
						break;
					default:
						buf[0] = 0xFF;
						buf[1] = 0xFF;
						break;
				}
				net_write(0, buf, 2);
			}		
		}
		
		if(is_ring(remote_config.phone)) {
			//管理员来电强制复位GPRS模块
			NVIC_SystemReset();				
		}
	}
}

uint8_t get_input(void) {
uint8_t input=0xFF;
	if(GPIOA->IDR & GPIO_IDR_IDR8) input &= ~0x01;
	if(GPIOC->IDR & GPIO_IDR_IDR9) input &= ~0x02;
	if(GPIOC->IDR & GPIO_IDR_IDR8) input &= ~0x04;
	if(GPIOC->IDR & GPIO_IDR_IDR7) input &= ~0x08;
	if(GPIOC->IDR & GPIO_IDR_IDR6) input &= ~0x10;
	if(GPIOB->IDR & GPIO_IDR_IDR15) input &= ~0x20;
	if(GPIOB->IDR & GPIO_IDR_IDR14) input &= ~0x40;
	if(GPIOB->IDR & GPIO_IDR_IDR13) input &= ~0x80;
	
	return input;
}

void set_output(uint8_t out) {
	(out & 0x01) ? (GPIOC->ODR |= GPIO_ODR_ODR12) : (GPIOC->ODR &= ~GPIO_ODR_ODR12);
	(out & 0x02) ? (GPIOD->ODR |= GPIO_ODR_ODR2) : (GPIOD->ODR &= ~GPIO_ODR_ODR2);	
	(out & 0x04) ? (GPIOB->ODR |= GPIO_ODR_ODR3) : (GPIOB->ODR &= ~GPIO_ODR_ODR3);	
	(out & 0x08) ? (GPIOB->ODR |= GPIO_ODR_ODR4) : (GPIOB->ODR &= ~GPIO_ODR_ODR4);	
	(out & 0x10) ? (GPIOB->ODR |= GPIO_ODR_ODR5) : (GPIOB->ODR &= ~GPIO_ODR_ODR5);	
	(out & 0x20) ? (GPIOB->ODR |= GPIO_ODR_ODR6) : (GPIOB->ODR &= ~GPIO_ODR_ODR6);	
	(out & 0x40) ? (GPIOB->ODR |= GPIO_ODR_ODR0) : (GPIOB->ODR &= ~GPIO_ODR_ODR0);	
}

uint8_t get_output(void) {
uint8_t input=0;
	if(GPIOC->IDR & GPIO_IDR_IDR12) input |= 0x01;
	if(GPIOD->IDR & GPIO_IDR_IDR2) input |= 0x02;
	if(GPIOB->IDR & GPIO_IDR_IDR3) input |= 0x04;
	if(GPIOB->IDR & GPIO_IDR_IDR4) input |= 0x08;
	if(GPIOB->IDR & GPIO_IDR_IDR5) input |= 0x10;
	if(GPIOB->IDR & GPIO_IDR_IDR6) input |= 0x20;
	if(GPIOB->IDR & GPIO_IDR_IDR0) input |= 0x40;
	
	return input;	
}
