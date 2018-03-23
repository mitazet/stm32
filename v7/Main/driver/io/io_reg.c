#include "io_reg.h"

void SetBit(__IO void* address, uint32_t bit){
	*((uint32_t*)address) |= bit;
}

void ClearBit(__IO void* address, uint32_t bit){
	*((uint32_t*)address) &= ~bit;
}

uint32_t ReadBit(__IO void* address, uint32_t bit){
	return *((uint32_t*)address) &= bit;
}

void ClearReg(__IO void* address)
{
	*((uint32_t*)address) = 0;
}

void WriteReg(__IO void* address, uint32_t data)
{
	*((uint32_t*)address) = data;
}

uint32_t ReadReg(__IO void* address)
{
	return *((uint32_t*)address);
}
