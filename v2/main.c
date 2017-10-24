#include "stm32f303x8.h"
#include "usart.h"

// Main -----------------------------------------------------------------------
int main(void)
{
	char c;

    InitUSART2();

	while(1){
		c = ReadUsart();
		WriteUsart(c);
	}
    
	return 0;
}
