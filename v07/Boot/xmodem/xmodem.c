#include "xmodem.h"
#include "usart_driver.h"

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a /* Ctrl-Z */

#define XMODEM_BLOCK_SIZE 128

// send NACK until data receive
static int XmodemWait(void)
{
    long cnt = 0;
    
    while(!UsartIsReadEnable()){
        if(++cnt >= 2000000){
            cnt = 0;
            UsartWrite(XMODEM_NAK);
        }
    }

    return 0;
}

// receive block
static int XmodemReadBlock(unsigned char block_number, char* buf)
{
    unsigned char c, block_num, check_sum;
    int i;

    block_num = UsartRead();
    if(block_num != block_number){
        return -1;
    }

    block_num ^= UsartRead();
    if(block_num != 0xff){
        return -1;
    }

    check_sum = 0;
    for(i=0; i<XMODEM_BLOCK_SIZE; i++){
        c = UsartRead();
        *(buf++) = c;
        check_sum += c;
    }

    check_sum ^= UsartRead();
    if(check_sum){
        return -1;
    }

    return i;
}

long XmodemRecv(char* buf)
{
    int r, receiving = 0;
    long size = 0;
    unsigned char c, block_number = 1;

    while(1){
        if(!receiving){
            XmodemWait();
        }

        c = UsartRead();

        if(c == XMODEM_EOT){
            UsartWrite(XMODEM_ACK);
            break;
        }else if(c == XMODEM_CAN){
            return -1;
        }else if(c == XMODEM_SOH){
            receiving++;
            r = XmodemReadBlock(block_number, buf);
            if(r < 0){
                UsartWrite(XMODEM_NAK);
            }else{
                block_number++;
                size += r;
                buf += r;
                UsartWrite(XMODEM_ACK);
            }
        }else{
            if(receiving){
                return -1;
            }
        }
    }

    return size;
}
