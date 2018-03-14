#include "lib.h"
#include "usart_driver.h"

char mygetc(void)
{
    char c = UsartRead();
    c = (c == '\r') ? '\n' : c;

    myputc(c);

    return c;
}

void myputc(char c)
{
    if(c == '\n'){
        UsartWrite('\r');
    }
    UsartWrite(c);
}

int gets(char* buf)
{
    int i=0;
    char c;

    do{
        c = mygetc();
        if(c == '\n'){
            c = '\0';
        }
        buf[i++] = c;
    }while(c);

    return i-1;
}

void* memset(void* s, int c, size_t len)
{
    char* p = s;

    for(; len > 0; len--){
        *(p++) = c;
    }

    return s;
}

void* memcpy(void* dst, const void* src, size_t len)
{
    char* d = dst;
    const char* s = src;

    for(; len > 0; len--){
        *(d++) = *(s++);
    }

    return dst;
}

int memcmp(const void* s1, const void* s2, size_t len)
{
    const char* p1 = s1;
    const char* p2 = s2;

    for(; len > 0; len--){
        if(*p1 != *p2){
            return (*p1 > *p2) ? 1 : -1;
        }
        p1++;
        p2++;
    }
    return 0;
}

int strlen(const char* str)
{
    int len;
    for(len=0; *str; str++, len++)
        ;
    return len;
}

char* strcpy(char* dst, const char* src)
{
    char* d = dst;

    for(; ; dst++, src++){
        *dst = *src;
        if(!*src) break;
    }

    return d;
}

int strcmp(const char* str1, const char* str2)
{
    while(*str1 || *str2){
        if(*str1 != *str2){
            return (*str1 > *str2) ? 1 : -1;
        }
        str1++;
        str2++;
    }
    return 0;
}

int strncmp(const char* str1, const char* str2, size_t len)
{
    while((*str1 || *str2) && (len > 0)){
        if(*str1 != *str2){
            return (*str1 > *str2) ? 1 : -1;
        }
        str1++;
        str2++;
        len--;
    }
    return 0;
}

