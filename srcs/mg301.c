#include	"mg301.h"

void mg_init(void) {
//	MG301_PWON();
	mg_soft_restart();
	is_gm301_on();
	
	disable_echo();
}

void mg_cmd(const char *str) {
	usart2_buf_clr();
	yputs(str);
	yputs("\r\n");
	delay(CMD_WAIT_TIME);
}

void mg_soft_restart(void) {
	mg_cmd("AT^SMSO");
	sleep(20);
}

uint8_t is_gm301_on(void) {
	mg_cmd("AT");
	if(strstr(get_usart2_buf(), "OK")) {
		return 1;
	}
	
	return 0;
}

void disable_echo(void) {
	mg_cmd("ATE0");
	delay(CMD_WAIT_TIME);
}

uint8_t is_rcv_nwtime(void) {
	if(strstr(get_usart2_buf(), "NWTIME") && strstr(get_usart2_buf(), "+")) {
		return 1;
	}
	return 0;
}

uint8_t set_profile(uint8_t id, const char *server, const char *apn, const char *user, const char *passwd) {
char str[128];
	sprintf(str, "%s=%d,conType,%s\n", "AT^SICS", id, "GPRS0");
	mg_cmd(str);
	
	sprintf(str, "%s=%d,apn,%s\n", "AT^SICS", id, apn);
	mg_cmd(str);

	sprintf(str, "%s=%d,user,%s\n", "AT^SICS", id, user);
	mg_cmd(str);

	sprintf(str, "%s=%d,passwd,%s\n", "AT^SICS", id, passwd);
	mg_cmd(str);

	sprintf(str, "%s=%d,srvType,%s\n", "AT^SISS", id, "Socket");
	mg_cmd(str);

	sprintf(str, "%s=%d,conId,%d\n", "AT^SISS", id, id);
	mg_cmd(str);
	
	sprintf(str, "%s=%d,address,\"socktcp://%s\"\n", "AT^SISS", id, server);
	mg_cmd(str);
	
	return 0;
}

uint8_t net_open(uint8_t id) {
char str[32];
uint8_t retry, ret=1;
	
	sprintf(str, "%s=%d\n", "AT^SISO", id);
	
	for(retry=0; retry<3; retry++) {
		mg_cmd("AT+CHUP");
		mg_cmd(str);
		sleep(3);
		mg_cmd("AT^SISO?");
		if(strstr(get_usart2_buf(), "\"4\"")) {
			ret = 0;
			break;
		}
		sleep(5);
	}
	return ret;
}

uint8_t net_write(uint8_t id, const char *buf, uint16_t len) {
char sisw[128] = "AT^SISW=0,";
uint8_t retry, ret=1;
uint16_t i;
	
	sprintf(sisw, "%s=%d,%d\n", "AT^SISW", id, len);
	for(retry=0; retry<3; retry++) {
		mg_cmd("AT+CHUP");
		mg_cmd(sisw);
		for(i=0; i<len; i++) {
			yputc(buf[i]);
		}
		sleep(3);
		if(strstr(get_usart2_buf(), "OK")) {
			ret = 0;
			break;
		}	
	}
	
	return ret;
}

uint8_t net_puts(uint8_t id, const char *msg) {
char sisw[128] = "AT^SISW=0,";
int len;
uint8_t retry, ret=1;
	
	len = strlen(msg);
	sprintf(sisw, "%s=%d,%d\n", "AT^SISW", id, len);
	for(retry=0; retry<3; retry++) {
		mg_cmd("AT+CHUP");
		mg_cmd(sisw);
		mg_cmd(msg);
		sleep(3);
		if(strstr(get_usart2_buf(), "OK")) {
			ret = 0;
			break;
		}	
	}
	
	return ret;
}

uint8_t net_read(uint8_t id, char *buf, uint16_t len) {
uint16_t num=0, retry;
char read[32]= "AT^SISR=0,", temp[32];
char *p;	
	
	for(retry=0; retry<3; retry++) {
//		mg_cmd("AT+CHUP");
		mg_cmd("AT^SISR=0,0");
		if((p=strstr(get_usart2_buf(), "SISR: 0,")) != NULL) {
			p += strlen("SISR: 0,");
			num = atoi(p);
			if(num == 0) {
				continue;
			}
			if((num > 0) && (num <= len)) {
				sprintf(temp, "%d\n", num);
				strcat(read, temp);
				mg_cmd(read);
				if((p = strstr(get_usart2_buf(), "SISR:")) != NULL) {
					while(*p++ != '\n');
					while(num--) {
						*buf++ = *p++;
					}
					*buf = 0;
				}
			}
			break;
		}
	}
	return num;
}

uint8_t net_close(uint8_t id) {
char sisc[32];
uint8_t retry, ret=1;
	
	sprintf(sisc, "%s=%d\n", "AT^SISC", id);
	for(retry=0; retry<3; retry++) {
		mg_cmd(sisc);
		sleep(3);
		if(strstr(get_usart2_buf(), "OK")) {
			ret = 0;
			break;
		}	
	}
	
	return ret;
}

uint8_t is_net_connected(uint8_t id) {
char findstr[32];
char *p;
uint8_t i, connected=0;	
	sprintf(findstr, "%s %d", "^SISO:", id);
	mg_cmd("AT^SISO?");
	if((p=strstr(get_usart2_buf(), findstr)) != NULL) {
		for(i=0; i<128; i++) {
			if(p[i] == 0x0a){
				p[i] = 0x00;
				break;
			}
		}
		sprintf(findstr, "%s", "\"4\",\"2\"");
		if(strstr(p, findstr)) {
			connected=1;
		}
	}		
	
	return connected;
}

uint8_t send_sms(char *phone, char *msg) {
char cmgs[32] = "AT+CMGS=";
	
	mg_cmd("AT+CMGF=1");
	mg_cmd("AT+CPMS=\"SM\",\"SM\",\"SM\"");
	mg_cmd("AT+CSCA=+8613800871500");
	strcat(cmgs, (char *)phone);
	mg_cmd(cmgs);
	while(!(strstr(get_usart2_buf(), ">")));
	mg_cmd(msg);
	mg_cmd("\x1A");
	
	return 0;
}

uint8_t is_ring(const char *phone) {
uint8_t ret=0;
	
	if(strstr(get_usart2_buf(), "RING")) {
		mg_cmd("AT+CLCC");
		if(strstr(get_usart2_buf(), phone) == NULL) {
			sleep(3);
		} else {
			ret = 1;
		}
		mg_cmd("AT+CHUP");		
	}
	
	return ret;
}

uint8_t get_rssi(void) {
char csq[32];
char i, j;
uint8_t rssi;	
	
	mg_cmd("AT+CSQ");
	if(strstr(get_usart2_buf(), "OK")) {
		memcpy(csq, get_usart2_buf(), 32);
		for(i=0; i<32; i++) {
			if(csq[i] == ':') {
				j=i;
			} else if(csq[i] == ',') {
				csq[i] = 0;
				rssi = (uint8_t)atoi(csq+j+1);
				break;
			}
		}
	}
	
	return rssi;
}
