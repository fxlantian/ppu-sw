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
	BOOTREG = 0x00000000;
	u32 retval=0;
	uart_set_cfg(0, 107);
	int_enable();
	printf("WIFI TEST!\n");
	IER |= (1<< INT_EMMC);
	printf("int open!\n");

	retval  = emmc_init_controller();	// Now initialize the controller
	if(retval) {
		printf("Could not initialize the host controller IP \n");
	}
	else 
		printf("The host controller IP initialize complete\n");

	//buff_write1  = (u8 *) malloc(MEM_SIZE);      //8192
	//buff_read1   = (u8 *) malloc(MEM_SIZE);  
	buff_write1  = (u8 *)0x22000000;      //8192
	buff_read1   = (u8 *)0x26000000;
    memset((void *)buff_read1 ,0x00,MEM_SIZE);
    memset((void *)buff_write1,0x00,MEM_SIZE);
    printf("TESTING  write and read for MMC/SD device\n");
    data_to_write = 0xF0;//IDMAC testing
	ii = 0;
	//for(ii = 0; ii<NO_OF_TEST_LOOPS;)
	//{
	    memset((void *)buff_read1,0x00,MEM_SIZE);
	    memset((void*)(buff_write1),data_to_write,SECTOR_SIZE);
	    memset((void*)(buff_write1+SECTOR_SIZE),0xaa,(MEM_SIZE-SECTOR_SIZE));
	    dma_ownbit_set();
	    //emmc_set_register(EMMC_REG_PLDMND,0x01);
        ret =  emmc_read_write_multiple_blocks(0, ii,NUM_OF_MUTLIPLE_BLOCKS , buff_write1, WRITE,(SECT_SIZE_SHIFT + 1));
	    if (ret) {
        	printf( "WRITE FAILED with %u value at %d write operation\n",ret,ii);
            //break;
	    }
       
		plat_delay(10000);
	    dma_ownbit_set();
	    //emmc_set_register(EMMC_REG_PLDMND,0x01);
		ret =  emmc_read_write_multiple_blocks(0, ii, NUM_OF_MUTLIPLE_BLOCKS  , buff_read1, READ,(SECT_SIZE_SHIFT + 1));
	    if (ret) {
            printf("READ FAILED with %u value at ii = %d\n",ret,ii);
           	//break;
		}
      
        if ((memcmp(buff_read1,buff_write1,( NUM_OF_MUTLIPLE_BLOCKS  * 512)) == 0)) {
   	        printf( "TEST SUCCESS for %d WRITE/READ Operation \n",ii);
       	}
       	else {
           	printf( "TEST FAILURE for %d  WRITE/READ operation \n",ii);
            for(i =0; i<(NUM_OF_MUTLIPLE_BLOCKS*SECTOR_SIZE);i++)
                printf("%02x",buff_read1[i]);
                printf("\n");
               	//break;
		}
		
		//ii = ii + NUM_OF_MUTLIPLE_BLOCKS;
/*	
		if(data_to_write == 0x8F)
			data_to_write = 0x80;
		else
		        data_to_write += 1;
*/
		//data_to_write += 1;
	emmc_dump_registers();
	printf("READ_WRITE TEST COMPLETED\n");
	return 0;
}