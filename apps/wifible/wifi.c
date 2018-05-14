#include <stdio.h>
#include "emmc.h"
#include "int.h"
#include "event.h"
#include "gpio.h"
#include "uart.h"
#define MEM_SIZE 8192
#define SECTOR_SIZE     512 //4 // 8 //16 // 32 //64 //128 //256 //512
#define TOT_NO_OF_SECTORS (MEM_SIZE/SECTOR_SIZE)
#define SECT_SIZE_SHIFT 9   //2 // 3 // 4 //5   //6  //7   //8   //9

#define WRITE 1
#define READ 0

#define NUM_OF_MUTLIPLE_BLOCKS  1
#define NO_OF_TEST_LOOPS  3  

int main(int argc, char const *argv[])
{
	int count,ii,jj,i;	
	//u8 buff_write1[MEM_SIZE];
	// u8 buff_read1[MEM_SIZE];
	u8 *buff_write1;
	u8 *buff_read1;
	u8 data_to_write;
	u32 ret;
	//BOOTREG = 0x00000000;
	u32 retval=0;
	//uart_set_cfg(0, 49);
	//int_enable();
	printf("WIFI TEST!\n");
	//IER |= (1<< INT_EMMC);
	//printf("int open!\n");

	//retval  = emmc_init_controller();	// Now initialize the controller
	emmc_dump_registers();
	printf("READ_WRITE TEST COMPLETED\n");
	return 0;
}