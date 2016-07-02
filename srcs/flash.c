#include	"flash.h"

void flash_write(uint32_t addr, uint8_t *data, uint16_t len) {
volatile uint16_t *paddr = (volatile uint16_t *)addr;
	
	if(len%2 == 1) {
		len += 1;
	}
	len /= 2;
	
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
	
	//PAGE ERASE
	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR |= addr;
	FLASH->CR |= FLASH_CR_STRT;
	while((FLASH->SR & FLASH_SR_BSY) != 0);
	if((FLASH->SR & FLASH_SR_EOP)!=0) {
		FLASH->SR |= FLASH_SR_EOP;
	}
	FLASH->CR &= ~FLASH_CR_PER;
	
	//WRITE
	FLASH->CR |= FLASH_CR_PG;
	
	while(len--) {
		*paddr++ = *(uint16_t *)data;
		data+=2;
	}
	
	while((FLASH->SR & FLASH_SR_BSY) != 0);
	if((FLASH->SR & FLASH_SR_EOP) != 0) {
		FLASH->SR |= FLASH_SR_EOP;
	}
	
	FLASH->CR &= ~FLASH_CR_PG;
	
	FLASH->CR |= FLASH_CR_LOCK;
}
