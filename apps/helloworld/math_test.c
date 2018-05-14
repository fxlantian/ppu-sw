//2016/11/29 
//Lei

#include <stdio.h>
// #include "gpio.h"
// #include "uart.h"
#include <math.h>

int main()
{
	volatile int a; 
	int length_groups = 0;
	for(length_groups = 0;length_groups<10;length_groups++)
	{
		printf("Hello World!\n");
  		a = cos( length_groups);
	}
}

