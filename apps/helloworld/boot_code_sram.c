#include <uart.h>
#include <utils.h>

#define COPY_SIZE 5 // The is the maximux size can be allocated 
#define ext_ram_addr 0x26000000

void jump_and_start(volatile int *ptr);

int uart_getdata(){
  int mid[4];
  int data = 0;
  for(int i = 0; i < 4; i++)
  {
    mid[i] = uart_getchar();
    data |= (mid[i] << (24 - i*8));
  }
  return data;
}

int main()
{
  BOOTREG = 0x00000000;
  unsigned int addr = 0;
  unsigned int data = 0;
  uart_set_cfg(0, 49); //100M: 125000
  
  uart_send("Please Upload Program via UART ...\n", 35);
  uart_wait_tx_done();

  addr = uart_getdata();
  unsigned int instr_ptr_t = addr;
  unsigned int addr_last = addr - 4;
  unsigned int data_ptr_t = 0;
  unsigned int instr_ptr_b = 0;
  while(addr != 0xffffffff)
  {
    data = uart_getdata();
    
    if(addr - addr_last != 0x04)
    {
          data_ptr_t = addr;
          instr_ptr_b = addr_last;
    }

    addr_last = addr;
    *(volatile int*)(addr) = data;
    addr = uart_getdata();
  }
  unsigned int data_ptr_b = addr_last;

  uart_send("Jumping to Instruction RAM\n", 27);
  uart_wait_tx_done();

  jump_and_start((volatile int *)(0x00000080));
}

void jump_and_start(volatile int *ptr)
{
  asm("jalr x0, %0\n"
      "nop\n"
      "nop\n"
      "nop\n"
      : : "r" (ptr) );
}
