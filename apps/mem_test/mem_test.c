//this code is used to check final elf files
//2016/10/27 
//Lei
//when using fpga simulation, malloc failed. Not clear why yet
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "memctl.h"
//#define COPY_SIZE 303 // The is the maximux size can be allocated 
#define COPY_SIZE 10 // The is the maximux size can be allocated 
#define ram_addr_start 0x80000000
#define ram_addr_stop  0x81000000

int main()
{
 // int* q;
 // memctl_init();
//  int * p;

//  if ( (p = (int *) malloc(sizeof(int)*COPY_SIZE)) == NULL){ 
//      printf("Allocation p(%d) Failed. Exit\n", COPY_SIZE);
//      while(1);
//  }

  //int * q;
  //if ( (q = (int *) malloc(sizeof(int)*COPY_SIZE)) == NULL){ 
  //    printf("Allocation q(%d) Failed. Exit\n", COPY_SIZE);
  //    while(1);
  //}

  volatile int * q;
  volatile int * p;
  int count=0;
//  printf("%x\n", *(volatile unsigned int*)ext_ram_addr);
  //  int * q = (int *) ext_ram_addr;

//  printf("0x%x\n", (int)p);
  //printf("start at 0x%x\n", ram_addr_start);

  uart_send("start\n", 6);
 
  for(q = (volatile int *)ram_addr_start; q < (volatile int *)ram_addr_stop; q++)
  {
      *q = q;
      if(*q != q){
       count++;
	
       
      // printf("Back-To-Back Error at addr: %08x, Read Data: %08x\n", q, *q);
     }
     uart_send("*", 1);
  }
  
/*
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

*/
//  while(1);

//  free(p);
  return 0;
}
