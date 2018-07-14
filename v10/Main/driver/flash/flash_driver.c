// FLASH ----------------------------------------------------------------------
#include "flash_driver.h"
#include "io_reg.h"

static FLASH_TypeDef* flashAddress = FLASH;

void FlashCreate(FLASH_TypeDef* flash_address)
{
    flashAddress = flash_address;
}

void FlashInit(void)
{
    WriteReg(&flashAddress->KEYR, FLASH_KEY1);
    WriteReg(&flashAddress->KEYR, FLASH_KEY2);
}

uint8_t FlashRead(uint8_t* address)
{
	return *address;
}

flash_result_t FlashWrite(uint16_t* address, uint16_t data)
{
    while(ReadBit(&flashAddress->SR, FLASH_SR_BSY));

    SetBit(&flashAddress->CR, FLASH_CR_PG);

    *address = data;

    while(ReadBit(&flashAddress->SR, FLASH_SR_BSY));

    if(!ReadBit(&flashAddress->SR, FLASH_SR_EOP)){
        return FLASH_RESULT_NG;
    }

    ClearBit(&flashAddress->SR, FLASH_SR_EOP);

	return FLASH_RESULT_OK;
}

flash_result_t FlashPageErase(uint8_t* address)
{
    while(ReadBit(&flashAddress->SR, FLASH_SR_BSY));

    SetBit(&flashAddress->CR, FLASH_CR_PER);

    WriteReg(&flashAddress->AR, (uint32_t)address);

    SetBit(&flashAddress->CR, FLASH_CR_STRT);

    while(ReadBit(&flashAddress->SR, FLASH_SR_BSY));

    if(!ReadBit(&flashAddress->SR, FLASH_SR_EOP)){
        return FLASH_RESULT_NG;
    }

    ClearBit(&flashAddress->SR, FLASH_SR_EOP);
	return FLASH_RESULT_OK;
}

flash_result_t FlashMassErase(void)
{
	return FLASH_RESULT_OK;
}
