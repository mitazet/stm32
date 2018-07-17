#include "usart_driver.h"
#include "printf.h"
#include "lib.h"
#include "xmodem.h"
#include "flash_driver.h"


// form Linker Script
extern void _main_code();
extern int  _flash_code;
extern int  _main_code_size;

static int dump(char* buf, long size)
{
    long i;
    
    if(size < 0){
       printf("no data.\n");
       return -1; 
    }
    for(i=0; i<size; i++){
        printf("%02X", buf[i]);
        if((i&0xf) == 15){
            printf("\n");
        }else{
            if((i&0xf) == 7) printf(" ");
            printf(" ");
        }
    }
    printf("\n");

    return 0;
}

static int erase_code(void)
{
    uint8_t* flash_addr = (uint8_t*)&_flash_code;
    uint32_t page_num_code = (uint32_t)&_main_code_size / FLASH_PAGE_SIZE_BYTE + 1; 
    
    printf("Code Area Address: 0x%X\n", flash_addr);
    printf("Page num of Code Area: %u\n", page_num_code);

    for(uint32_t i=0; i < page_num_code; i++){
        if(FlashPageErase(flash_addr) != FLASH_RESULT_OK){
            printf("erase error occured!!\n");
            return -1;
        }

        flash_addr += FLASH_PAGE_SIZE_BYTE;
    }

    return 0;
}

static int write_code(char* buf, long size)
{
    long i;

    if(size < 0){
        printf("no data.\n");
        return -1;
    }
    
    uint16_t* flash_addr    = (uint16_t*)&_flash_code;
    uint16_t* data_addr     = (uint16_t*)buf;

    FlashInit();

    if(erase_code() != 0){
        return -1;
    }

    for(i=0; i<size; i++){
        if(FlashWrite(flash_addr, *data_addr) != FLASH_RESULT_OK){
            printf("write error occured!!\n");
            return -1;
        }

        flash_addr++;
        data_addr++;
    }

    return 0;
}

static void load_code(char *buf)
{
    uint8_t* flash_addr = (uint8_t*)&_flash_code;
	uint32_t main_code_size = (uint32_t)&_main_code_size;

    for(uint32_t i=0; i<main_code_size; i++){
        *buf = FlashRead(flash_addr);
        buf++;
        flash_addr++;
    }
}

static void wait()
{
    volatile long i;
    for(i=0; i<300000; i++)
        ;
}

// Main -----------------------------------------------------------------------
int main(void)
{
    static char buf[16];
    static long size = -1;
    static unsigned char *loadbuf = NULL;
    extern int _app_vector; // from Linker Script
    
    __disable_irq(); // disable interrupt

    UsartInit();

    printf("zloader started.\n");

    while(1){
        printf("zload> ");
        gets(buf);

        if(!strcmp(buf, "loadx")){
            loadbuf = (char*)(&_app_vector);
            size = XmodemRecv(loadbuf);
            wait();
            if(size < 0){
                printf("\nXMODEM receive error!\n");
            }else{
                printf("\nXMODEM receive succeeded.\n");
            }
        }else if(!strcmp(buf, "loadf")){
            loadbuf = (char*)(&_app_vector);
            load_code(loadbuf);
        }else if(!strcmp(buf, "dump")){
            printf("size: %d\n", size);
            dump(loadbuf, size);
        }else if(!strcmp(buf, "write")){
            int write_result = write_code(loadbuf, size);
            if(write_result != 0){
                printf("\nWrite Code error!\n");
            }else{
                printf("\nWrite Code is succeeded.\n");
            }
        }else if(!strcmp(buf, "run")){
            _main_code();
        }else{
            printf("unknown.\n");
        }
    }

	return 0;
}
