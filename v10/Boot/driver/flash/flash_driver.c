// FLASH ----------------------------------------------------------------------
#include "flash_driver.h"
#include "io_reg.h"
#include <stdbool.h>
#include "printf.h"

// from Linker Script
extern int _flash_addr;
extern int _flash_size;

// Default:FLASH
static FLASH_TypeDef* flashAddress = FLASH;

static bool is_flash_area(uint32_t address, uint32_t size)
{
	uint32_t flash_start = (uint32_t)&_flash_addr;
	uint32_t flash_end   = (uint32_t)&_flash_addr + (uint32_t)&_flash_size;

	if((address < flash_start) || (address + size > flash_end)){
		return false;
	}
	return true;
}

static bool is_flash_locked(void)
{
    if(ReadBit(&flashAddress->CR, FLASH_CR_LOCK)){
        return true;
    }
	return false;
}

static bool is_flash_busy(void)
{
    if(ReadBit(&flashAddress->SR, FLASH_SR_BSY)){
		return true;
	}
	return false;
}

static flash_result_t check_flash_eop(void)
{
    if(!ReadBit(&flashAddress->SR, FLASH_SR_EOP)){
        return FLASH_RESULT_NG;
    }

    ClearBit(&flashAddress->SR, FLASH_SR_EOP);

	return FLASH_RESULT_OK;
}

static void flash_unlock(void)
{
    WriteReg(&flashAddress->KEYR, FLASH_KEY1);
    WriteReg(&flashAddress->KEYR, FLASH_KEY2);
}

static void flash_write(uint16_t* address, uint16_t data)
{
    SetBit(&flashAddress->CR, FLASH_CR_PG);
    *address = data;
}

static void flash_page_erase(uint8_t* address)
{
    SetBit(&flashAddress->CR, FLASH_CR_PER);
    WriteReg(&flashAddress->AR, (uint32_t)address);
    SetBit(&flashAddress->CR, FLASH_CR_STRT);
}

void FlashCreate(FLASH_TypeDef* flash_address)
{
    flashAddress = flash_address;
}

void FlashInit(void)
{
	flash_unlock();
}

uint8_t FlashRead(uint8_t* address)
{
	if(!is_flash_area((uint32_t)address, sizeof(*address))){
		return 0;
	}

	return *address;
}

flash_result_t FlashWrite(uint16_t* address, uint16_t data)
{
	if(!is_flash_area((uint32_t)address, sizeof(*address))){
		return FLASH_RESULT_NG;
	}

    if(is_flash_locked()){
        return FLASH_RESULT_NG;
    }

    while(is_flash_busy());

	flash_write(address, data);

    while(is_flash_busy());

	return check_flash_eop();
}

flash_result_t FlashPageErase(uint8_t* address)
{
	if(!is_flash_area((uint32_t)address, sizeof(*address))){
		return FLASH_RESULT_NG;
	}

    if(is_flash_locked()){
        return FLASH_RESULT_NG;
    }

    while(is_flash_busy());

	flash_page_erase(address);

    while(is_flash_busy());

	return check_flash_eop();
}

flash_result_t FlashMassErase(void)
{
	return FLASH_RESULT_OK;
}
