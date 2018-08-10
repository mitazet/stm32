// FLASH ----------------------------------------------------------------------
#include "flash_driver.h"
#include "io_reg.h"
#include <stdbool.h>
#include "printf.h"


bool FlashDriver::is_area(uint32_t address, uint32_t size)
{
    if((address < flash_start_) || (address + size > flash_end_)){
        return false;
    }
    return true;
}

bool FlashDriver::is_locked(void)
{
    if(ReadBit(&flash_base_->CR, FLASH_CR_LOCK)){
        return true;
    }
    return false;
}

bool FlashDriver::is_busy(void)
{
    if(ReadBit(&flash_base_->SR, FLASH_SR_BSY)){
        return true;
    }
    return false;
}

flash_result_t FlashDriver::check_eop(void)
{
    if(!ReadBit(&flash_base_->SR, FLASH_SR_EOP)){
        return FLASH_RESULT_NG;
    }

    ClearBit(&flash_base_->SR, FLASH_SR_EOP);

    return FLASH_RESULT_OK;
}

void FlashDriver::unlock(void)
{
    WriteReg(&flash_base_->KEYR, FLASH_KEY1);
    WriteReg(&flash_base_->KEYR, FLASH_KEY2);
}

void FlashDriver::write(uint16_t* address, uint16_t data)
{
    SetBit(&flash_base_->CR, FLASH_CR_PG);
    *address = data;
}

void FlashDriver::page_erase(uint8_t* address)
{
    SetBit(&flash_base_->CR, FLASH_CR_PER);
    WriteReg(&flash_base_->AR, (uint32_t)address);
    SetBit(&flash_base_->CR, FLASH_CR_STRT);
}

FlashDriver::FlashDriver()
{
#ifndef DEBUG_GTEST
    flash_base_  = FLASH;
    flash_start_ = _flash_addr;
    flash_end_   = _flash_addr + _flash_size;
#endif
}

void FlashDriver::SetBase(FLASH_TypeDef* address, uint32_t start, uint32_t end)
{
    flash_base_  = address;
    flash_start_ = start;
    flash_end_   = end;
}

void FlashDriver::Init(void)
{
    unlock();
}

uint8_t FlashDriver::Read(uint8_t* address)
{
    if(!is_area((uint32_t)address, sizeof(*address))){
        return 0;
    }

    return *address;
}

flash_result_t FlashDriver::Write(uint16_t* address, uint16_t data)
{
    if(!is_area((uint32_t)address, sizeof(*address))){
        return FLASH_RESULT_NG;
    }

    if(is_locked()){
        return FLASH_RESULT_NG;
    }

    while(is_busy());

    write(address, data);

    while(is_busy());

    return check_eop();
}

flash_result_t FlashDriver::PageErase(uint8_t* address)
{
    if(!is_area((uint32_t)address, sizeof(*address))){
        return FLASH_RESULT_NG;
    }

    if(is_locked()){
        return FLASH_RESULT_NG;
    }

    while(is_busy());

    page_erase(address);

    while(is_busy());

    return check_eop();
}
