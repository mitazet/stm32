#include "printf.h"
#include "lib.h"

typedef void (*putcf)(char);
static putcf stdout_putf = myputc;

#ifdef PRINTF_LONG_SUPPORT

static void uli2a(unsigned long int num, unsigned int base, int uc, char* bf)
{
    int n = 0;
    unsigned int d = 1;

    while(num/d >= base){
        d *= base;
    }

    while(d != 0){
        int dgt = num/d;
        num %= d;
        d /= base;
        if(n || dgt>0 || d==0){
            //convert number to chractor 0-F(f)
            *bf = dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10);
            bf++;
            n++;
        }
    }
    *bf = '\0';
}

static void li2a(long num, char* bf)
{
    if(num < 0){
        num = -num;
        *bf = '-';
        bf++;
    }
    uli2a(num, 10, 0, bf);
}

#endif

static void ui2a(unsigned int num, unsigned int base, int uc, char* bf)
{
    int n = 0;
    unsigned int d = 1;

    while(num/d >= base){
        d *= base;
    }

    while(d != 0){
        int dgt = num/d;
        num %= d;
        d /= base;
        if(n || dgt > 0 || d==0){
            *bf = dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10);
            bf++;
            n++;
        }
    }
    *bf = '\0';
}

static void i2a(int num, char* bf)
{
    if(num < 0){
        num = -num;
        *bf = '-';
        bf++;
    }
    ui2a(num, 10, 0, bf);
}

static int a2d(char ch)
{
    if(ch >= '0' && ch <= '9'){
        return ch-'0';
    }else if(ch >= 'a' && ch <= 'f'){
        return ch-'a'+10;
    }else if(ch >= 'A' && ch <= 'F'){
        return ch-'A'+10;
    }else{
        return -1;
    }
}

static char a2i(char ch, char** src, int base, int* nump)
{
    char* p = *src;
    int num = 0;
    int digit;

    while((digit=a2d(ch)) >= 0){
        if(digit>base){
            break;
        }
        num = num*base+digit;
        ch = *p;
        p++;
    }
    *src = p;
    *nump = num;

    return ch;
}

static void putchw(putcf putf, int n, char z, char* bf)
{
    char fc = z ? '0' : ' ';
    char ch;
    char* p = bf;

    while(*p++ && n>0){
        n--;
    }

    while(n-- > 0){
        putf(fc);
    }

    while((ch=*bf) != '\0'){
        putf(ch);
        bf++;
    }
}

void tfp_format(putcf putf, char *fmt, va_list va)
{
    char bf[12];
    char ch;

    while((ch=*fmt) != '\0'){
        fmt++;
        if(ch != '%'){
            putf(ch);
            // next charactor
            continue;
        }            

        // %?
        char lz = 0;
        int w = 0;
#ifdef PRINTF_LONG_SUPPORT
        char lng = 0;
#endif

        ch = *fmt;
        fmt++;  // next charactor

        if(ch == '0'){
            // %0
            ch = *fmt;
            fmt++;
            lz = 1;
        }

        if(ch >= '0' && ch <= '9'){
            ch = a2i(ch, &fmt, 10, &w);
        }

#ifdef PRINTF_LONG_SUPPORT
        if(ch == 'l'){
            ch = *fmt;
            fmt++;
            lng = 1;
        }
#endif
        switch(ch){
            case '\0':
                goto abort;
            case 'u':
#ifdef PRINTF_LONG_SUPPORT
                if(lng){
                    uli2a(va_arg(va, unsigned long int), 10, 0, bf);
                }else
#endif
                {
                    ui2a(va_arg(va, unsigned int), 10, 0, bf);
                }
                putchw(putf, w, lz, bf);
                break;
                
            case 'd':
#ifdef PRINTF_LONG_SUPPORT
                if(lng){
                    li2a(va_arg(va, unsigned long int), bf);
                }else
#endif
                {
                    i2a(va_arg(va, int), bf);
                }
                putchw(putf, w, lz, bf);
                break;

            case 'x':
            case 'X':
#ifdef PRINTF_LONG_SUPPORT
                if(lng){
                    uli2a(va_arg(va, unsigned long int), 16, (ch=='X'), bf);
                }else
#endif
                {
                    ui2a(va_arg(va, unsigned int), 16, (ch=='X'), bf);
                }
                putchw(putf, w, lz, bf);
                break;

            case 'c':
                putf((char)(va_arg(va, int)));
                break;

            case 's':
                putchw(putf, w, 0, va_arg(va, char*));
                break;

            case '%':
                putf(ch);
            default:
                break;
        }
    }
    abort:;
}

void init_printf(void (*putf)( char))
{
    stdout_putf = putf;
}

void tfp_printf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    tfp_format(stdout_putf, fmt, va);
    va_end(va);
}
