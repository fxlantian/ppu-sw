//2017/1/19 


#include <stdio.h>
#include "emmc.h"
//#include "int.h"
#include "event.h"
#include "gpio.h"
#include "memctl.h"
#include "uart.h"
#define MEM_SIZE 8192
#define SECTOR_SIZE     512 //4 // 8 //16 // 32 //64 //128 //256 //512
#define TOT_NO_OF_SECTORS (MEM_SIZE/SECTOR_SIZE)
#define SECT_SIZE_SHIFT 9   //2 // 3 // 4 //5   //6  //7   //8   //9

#define WRITE 1
#define READ 0


#define NUM_OF_MUTLIPLE_BLOCKS  1
#define NO_OF_TEST_LOOPS  3  

int main()
{
	int a,i;
	//BOOTREG = INSTR_RAM_BASE_ADDR;
	//memctl_init();
//	uart_set_cfg(0, 49);

u32 cmd_tx_done;
u32 retval=0;
//set_gpio_pin_direction(2, 1);
//set_gpio_pin_value(2, 0);

//int_enable();

//EER = 0x00004000; // enable emmc events;
//IER = 0x00008000; // enable emmc  interrupts
//IER |= (1<< INT_EMMC);
//printf("int open!\n");
retval  = emmc_init_controller();	// Now initialize the controller
	if(retval) {
	printf("Could not initialize the host controller IP \n");
		return retval;
	}


/************************************************************************************************	
This test is performed to check whether Multiple block write and read is supported for odd and 
even number of multiple block transfers. This test is introduced here since some MMC and SD
card show block write skip problem for odd number of block transfers
************************************************************************************************/	
u32 ret,REG;
//emmc_set_bits(EMMC_REG_CTYPE, 0x00000001);
/************************************************************************************************	
This test is Erase function validataion for MMC and SD cards.
@paramerter slot
@parameter  Erase_block_start address/sector index (For MMC4.2 and SD2.0 this is sector index for other cards it is byte address)
@parameter  Erase_block_End address/sector index (For MMC4.2 and SD2.0 this is sector index for other cards it is byte address)
************************************************************************************************/	
#if 0 
{

printf("Starting Erase......\n");

ret = emmc_erase(0,0x0,0x0);
if(ret)
        printf("Erase operation failed with %d\n",ret);
}
#endif

{

		 
 		 int count,ii,jj,i;	
		 //u8 buff_write1[MEM_SIZE];
		// u8 buff_read1[MEM_SIZE];
		 u8 *buff_write1;
		 u8 *buff_read1;
		 u8 data_to_write;

		
		//buff_write1  = (u8 *) malloc(MEM_SIZE);      //8192
		//buff_read1   = (u8 *) malloc(MEM_SIZE);  
		buff_write1  = (u8 *)0x22000000;      //8192
		buff_read1   = (u8 *)0x26000000;
        memset((void *)buff_read1 ,0x00,MEM_SIZE);
        memset((void *)buff_write1,0x00,MEM_SIZE);
        printf("TESTING  write and read for MMC/SD device\n");
        data_to_write = 0xF0;//IDMAC testing
	for(ii = 0; ii<NO_OF_TEST_LOOPS;)
	{
	    memset((void *)buff_read1,0x00,MEM_SIZE);
	    memset((void*)(buff_write1),data_to_write,SECTOR_SIZE);
	    memset((void*)(buff_write1+SECTOR_SIZE),0xaa,(MEM_SIZE-SECTOR_SIZE));
	    dma_ownbit_set();
 
        	ret =  emmc_read_write_multiple_blocks(0, ii,NUM_OF_MUTLIPLE_BLOCKS , buff_write1, WRITE,(SECT_SIZE_SHIFT + 1));
//        	ret =  synopmob_read_write_multiple_blocks(0, 0,NUM_OF_MUTLIPLE_BLOCKS , buff_write1, WRITE,(SECT_SIZE_SHIFT + 1));
	        if (ret) {
        	        printf( "WRITE FAILED with %u value at %d write operation\n",ret,ii);
                	break;
	        }
       
		plat_delay(10000);
	    
		ret =  emmc_read_write_multiple_blocks(0, ii, NUM_OF_MUTLIPLE_BLOCKS  , buff_read1, READ,(SECT_SIZE_SHIFT + 1));
//	    ret =  synopmob_read_write_multiple_blocks(0, 0, NUM_OF_MUTLIPLE_BLOCKS  , buff_read1, READ,(SECT_SIZE_SHIFT + 1));
	        if (ret) {
        	        printf("READ FAILED with %u value at ii = %d\n",ret,ii);
                	break;
		}
      
      /*  for(i=0;i<SECTOR_SIZE;i++){
       		printf("fifo data %x : %x",*(EMMC_REG_FIFODAT++));
		
       }
     *//*
       for( i=0;i<SECTOR_SIZE;i++){
			printf("address %d : %x \n",i,*(buff_read1+i));
       }
*/

       #if 1

	        if ((memcmp(buff_read1,buff_write1,( NUM_OF_MUTLIPLE_BLOCKS  * 512)) == 0)) {
        	        printf( "TEST SUCCESS for %d WRITE/READ Operation \n",ii);
        	}
        	else {
                	printf( "TEST FAILURE for %d  WRITE/READ operation \n",ii);
	                for(i =0; i<(NUM_OF_MUTLIPLE_BLOCKS*SECTOR_SIZE);i++)
        	              printf("%02x",buff_read1[i]);
			      printf("\n");
                	break;
		        }
	   #endif        
		
		ii = ii + NUM_OF_MUTLIPLE_BLOCKS;
/*	
		if(data_to_write == 0x8F)
			data_to_write = 0x80;
		else
		        data_to_write += 1;
*/
		//data_to_write += 1;
	}
	printf("READ_WRITE TEST COMPLETED\n");
}




	
return 0;

}



