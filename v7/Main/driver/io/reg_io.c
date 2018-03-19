#include "reg_io.h"

void RegClear(uint32_t* address)
{
	*address = 0;
}

void RegWrite(uint32_t* address, uint32_t data)
{
	*address |= data;
}

uint32_t RegRead(uint32_t* address)
{
	return *address;
}

