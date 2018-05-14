//this code is used to check final elf files
//2016/10/27 
//Lei
//when using fpga simulation, malloc failed. Not clear why yet
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "memctl.h"
#include "bench.h"
//#define COPY_SIZE 303 // The is the maximux size can be allocated 
#define COPY_SIZE 10 // The is the maximux size can be allocated 
#define ram_addr_start 0x22000000
#define ram_addr_stop  0x27000000

int main()
{
  memctl_init();
  testresult_t result;
  test_mem(result);
  printf("ALL ERROR: %d\n", result->errors);
  return 0;
}

void test_spi0(testresult_t *result)
{
  *(volatile int*) (SPI_REG_CLKDIV) = 0x4;
  int id = 0;
  spi_setup_cmd_addr(0x9F, 8, 0, 0);
  spi_set_datalen(32);
  spi_start_transaction(SPI_CMD_RD, SPI_CSN0);
  spi_read_fifo(&id, 32);
  if ((id != 0x0120184D) && (id != 0x0102194D)) {
      uart_send("ERROR: Spansion SPI flash not found\n", 36);
      result->errors += 1;
    }
}

void test_mem(testresult_t *result)
{
  volatile int * q;
  volatile int * p;
  int count=0;
  printf("start at 0x%x\n", ram_addr_start);
  
  for(q = (volatile int *)ram_addr_start; q < (volatile int *)ram_addr_stop; q++)
    {
      *q = q;
      if(*q != q){
       count++;
       printf("Back-To-Back Error at addr: %08x, Read Data: %08x\n", q, *q);
     }
    }
  printf("Back-To-Back test complete\n");
  for(q = (volatile int *)ram_addr_start; q < (volatile int *)ram_addr_stop; q++)
     *q = q;
  
  for(q = (volatile int *)ram_addr_start; q < (volatile int *)ram_addr_stop; q++)
     if(*q != q){
       count++;
       printf("Continuous Error at addr: %08x, Read Data: %08x\n", q, *q);
     }
  printf("Continuous test complete\n");
  p = (volatile int *)ram_addr_stop;
  for(q = (volatile int *)ram_addr_start; q < (volatile int *)ram_addr_stop; q++)
     {
      *q = *p;
      if(*q != *p)
      {
        count++;
       printf("Move Error at addr: %08x, Read Data: %08x\n", q, *q);
      }
      p--;
    }
  printf("Move test complete\n");
  printf("Error count number: %d\n",count);
  result->errors += count;
}