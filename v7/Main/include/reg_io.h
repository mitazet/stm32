#ifndef __REG_IO_H__
#define __REG_IO_H__

extern void RegClear(uint32_t* address);
extern void RegWrite(uint32_t* address, uint32_t data);
extern uint32_t RegRead(uint32_t* address);

#endif 
