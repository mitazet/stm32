#include "usart_driver.h"
#include "printf.h"
#include "lib.h"
#include "xmodem.h"
#include "elf.h"

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

static void wait()
{
    volatile long i;
    for(i=0; i<300000; i++)
        ;
}

// Main -----------------------------------------------------------------------
int main(void)
{
    UsartInit();
    //init_printf(myputc);

    static char buf[16];
    static long size = -1;
    static unsigned char *loadbuf = NULL;
    extern int _buffer_start;

    printf("zload (zet boot loader) started.\n");

    while(1){
        printf("zload> ");
        gets(buf);

        if(!strcmp(buf, "load")){
            loadbuf = (char*)(&_buffer_start);
            size = XmodemRecv(loadbuf);
            wait();
            if(size < 0){
                printf("\nXMODEM receive error!\n");
            }else{
                printf("\nXMODEM receive succeeded.\n");
            }
        }else if(!strcmp(buf, "dump")){
            printf("size: %d\n", size);
            dump(loadbuf, size);
        }else if(!strcmp(buf, "run")){
            ElfLoad(loadbuf);
        }else{
            printf("unknown.\n");
        }
    }

	return 0;
}
