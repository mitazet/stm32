// FLASH ----------------------------------------------------------------------
#include "flash_driver.h"
#include "io_reg.h"
#include <stdbool.h>
#include "printf.h"




bool FlashDriver::is_area(uint32_t address, uint32_t size)
{
    if((address < start_) || (address + size > end_)){
        return false;
    }
    return true;
}

bool FlashDriver::is_locked(void)
{
    if(ReadBit(&address_->CR, FLASH_CR_LOCK)){
        return true;
    }
    return false;
}

bool FlashDriver::is_busy(void)
{
    if(ReadBit(&address_->SR, FLASH_SR_BSY)){
        return true;
    }
    return false;
}

flash_result_t FlashDriver::check_eop(void)
{
    if(!ReadBit(&address_->SR, FLASH_SR_EOP)){
        return FLASH_RESULT_NG;
    }

    ClearBit(&address_->SR, FLASH_SR_EOP);

    return FLASH_RESULT_OK;
}

void FlashDriver::unlock(void)
{
    WriteReg(&address_->KEYR, FLASH_KEY1);
    WriteReg(&address_->KEYR, FLASH_KEY2);
}

void FlashDriver::write(uint16_t* address, uint16_t data)
{
    SetBit(&address_->CR, FLASH_CR_PG);
    *address = data;
}

void FlashDriver::page_erase(uint8_t* address)
{
    SetBit(&address_->CR, FLASH_CR_PER);
    WriteReg(&address_->AR, (uint32_t)address);
    SetBit(&address_->CR, FLASH_CR_STRT);
}

FlashDriver::FlashDriver(FLASH_TypeDef* address, uint32_t start, uint32_t end)
{
    address_ = address;
    start_   = start;
    end_     = end;
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

flash_result_t FlashDriver::MassErase(void)
{
    return FLASH_RESULT_OK;
}
