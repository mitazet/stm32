#ifndef __FLASH_DRIVER__
#define __FLASH_DRIVER__

#include "stm32f303x8.h"

#define FLASH_PAGE_SIZE_BYTE 2048

#ifndef DEBUG_GTEST
// from Linker Script
extern int _flash_addr;
extern int _flash_size;
#endif

typedef enum{
    FLASH_RESULT_NG,
    FLASH_RESULT_OK,
}flash_result_t;

class FlashDriver
{
    public:
#ifndef DEBUG_GTEST
        FlashDriver(FLASH_TypeDef* address = FLASH, uint32_t start = _flash_addr, uint32_t end = _flash_addr + _flash_size);
#else
        FlashDriver(FLASH_TypeDef* address, uint32_t start, uint32_t end);
#endif
        void Init(void);
        uint8_t Read(uint8_t* address);
        flash_result_t Write(uint16_t* address, uint16_t data);
        flash_result_t PageErase(uint8_t* address);
        flash_result_t MassErase(void);

    private:
        FLASH_TypeDef* address_;
        uint32_t start_;
        uint32_t end_;

        bool is_area(uint32_t address, uint32_t size);
        bool is_locked(void);
        bool is_busy(void);
        flash_result_t check_eop(void);
        void unlock(void);
        void write(uint16_t* address, uint16_t data);
        void page_erase(uint8_t* address);
};

#endif
