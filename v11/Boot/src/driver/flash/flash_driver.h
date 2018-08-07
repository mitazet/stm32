#ifndef __FLASH_DRIVER__
#define __FLASH_DRIVER__

#include "stm32f303x8.h"

#define FLASH_PAGE_SIZE_BYTE 2048

typedef enum{
    FLASH_RESULT_NG,
    FLASH_RESULT_OK,
}flash_result_t;

extern void FlashCreate(FLASH_TypeDef* flash_address, uint32_t start_address, uint32_t end_address);
extern void FlashInit(void);
extern uint8_t FlashRead(uint8_t* address);
extern flash_result_t FlashWrite(uint16_t* address, uint16_t data);
extern flash_result_t FlashPageErase(uint8_t* address);
extern flash_result_t FlashMassErase(void);

#endif
