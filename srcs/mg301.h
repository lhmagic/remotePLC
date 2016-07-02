#ifndef		__MG301_H__
#define		__MG301_H__

#include	"bsp.h"

#define		CMD_WAIT_TIME				50


//public
void mg_init(void);
void mg_soft_restart(void) ;
uint8_t is_gm301_on(void);
void disable_echo(void);
uint8_t is_rcv_nwtime(void);
void update_time(void);
uint8_t set_profile(uint8_t id, const char *server, const char *apn, const char *user, const char *passwd);
uint8_t net_open(uint8_t id);
uint8_t net_write(uint8_t id, const char *buf, uint16_t len);
uint8_t net_puts(uint8_t id, const char *msg);
uint8_t net_read(uint8_t id, char *buf, uint16_t len);
uint8_t net_close(uint8_t id);
uint8_t is_net_connected(uint8_t id);
uint8_t send_sms(char *phone, char *msg);
uint8_t is_ring(const char *phone);
uint8_t get_rssi(void);

//private
static void mg_cmd(const char *str);

#endif		//__MG301_H__
