#ifndef __IO_REG_H__
#define __IO_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f303x8.h"

extern void SetBit(__IO void* address, uint32_t bit);
extern void ClearBit(__IO void* address, uint32_t bit);
extern uint32_t ReadBit(__IO void* address, uint32_t bit);
extern void ClearReg(__IO void* address);
extern void WriteReg(__IO void* address, uint32_t data);
extern uint32_t ReadReg(__IO void* address);

#ifdef __cplusplus
}
#endif

#endif 
