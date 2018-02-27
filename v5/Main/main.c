#include "printf.h"

int main(void)
	__attribute__ ((section (".entry_point")));

// Main -----------------------------------------------------------------------
int main(void)
{
    printf("Hello World.\n");

    while(1);

	return 0;
}
