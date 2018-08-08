#ifndef __LIB_H__
#define __LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef void (*putcf)(char);
typedef char (*getcf)(void);

extern void init_myputc(void (*putf)(char));
extern void init_mygetc(char (*getf)(void));
extern char mygetc(void);
extern void myputc(char c);
extern int gets(char *buf);
extern void* memset(void* s, int c, size_t len);
extern void* memcpy(void* dst, const void* src, size_t len);
extern int memcmp(const void* s1, const void* s2, size_t len);
extern int strlen(const char* str);
extern char* strcpy(char* dst, const char* src);
extern int strcmp(const char* str1, const char* str2);
extern int strncmp(const char* str1, const char* str2, size_t len);

#ifdef __cplusplus
}
#endif

#endif
