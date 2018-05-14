// Copyright 2015 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the ?°„License?°¿); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an ?°„AS IS?°¿ BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "ann.h"
#include <utils.h>
#include "stdlib.h"
#include <bench.h>
#include <stdio.h>
#include "int.h"
#include "uart.h"
#include "event.h"
#include "../testANN.h"
#include "malloc.h"
#include "memctl.h"

volatile int count = 0;
int* Dst_Addr;
#define ahb_pl1 0x28004000
#define ahb_pl2 0x28004004
#define ahb_pl3 0x28004008
#define ahb_pl4 0x2800400c
#define ahb_pl5 0x28004010
#define DMA_BLOCK_temp 16
#define config_addr 0x22000000
#define data_addr   0x26000000
int main() {
      
      BOOTREG = 0x00000000;
      memctl_init();

      uart_set_cfg(0,49);
      uart_send("start..\n",8);
      uart_wait_tx_done();
        int mstatus;
 
  asm volatile ("csrr %0, mstatus": "=r" (mstatus));
  printf("move data complete.22222222222222\n");
  mstatus |= 0x01;
   printf("move data complete.22222222222222\n");
  asm volatile ("csrw mstatus, %0" : /* no output */ : "r" (mstatus));
    printf("move data complete.22222222222222\n");
      int pvalue=0;
      *(volatile int*) (ahb_pl1)  = 10;
      pvalue = *(volatile int*)(ahb_pl1);
      printf("master 1 pvalue is %d\n", pvalue);

      int move_num;
      int *move_ptr = (int*)config_addr;
      move_num = IM_DEPTH0 + WEIGTH_DEPTH0 * 2 + BIAS_DEPTH0 / 4 + 128;
      int mi;
      printf("moving config array number: %d ..\n", move_num);
      for (int i = 0;i < move_num; i++)
      {
        *(move_ptr + i) = Ann_Config[i];
      }
      for (int i = 0; i < move_num; i++)
      {
        if(*(move_ptr + i) != Ann_Config[i]){
          printf("move data error at %d\n",i);
          while(1);
        }
        //printf("0x%08x,\n", *(move_ptr + i));
      }
      move_ptr = (int*)data_addr;
      move_num = NPU_DATAIN_DEPTH0 / 4 * 4;
      printf("moving data array number: %d ..\n", move_num);
      for (int i = 0;i < move_num; i++)
      {
        *(move_ptr + i) = data_input[i];
      }
      for (int i = 0; i < move_num; i++)
      {
        if(*(move_ptr + i) != data_input[i]){
          printf("move data error at %d\n",i);
          while(1);
        }
        //printf("0x%08x,\n", *(move_ptr + i));
      }
      printf("move data complete.\n");
      //while(1);
      //Dst_Addr = (int*)malloc(sizeof(int) * NPU_DATAOUT_DEPTH0);
      Dst_Addr = 0x27000000;
      if(Dst_Addr == 0)
      {
        printf("Dst_Addr malloc wrong!\n");
        while(1);
      }
         
      init_pro(IM_DEPTH0, WEIGTH_DEPTH0, BIAS_DEPTH0, config_addr, 8);   
      IER |= (1<<16);      
      
      while(1) {
        if(count==1)
          break;
         }
     
   
      int tmp_value_RST;
      int tmp_value_CHK; 
      int *j=(int*)Dst_Addr;  
      int *i=(int*)data_check; 
      int  ok=0;
      while(1){
        ok=0;
      for(int k=0; k < NPU_DATAOUT_DEPTH0; k= k+1){

          tmp_value_RST = *(j+k);
          tmp_value_CHK = *(i+k);
  
          if(tmp_value_RST == tmp_value_CHK)
          //  printf("ERROR line is %0d expect 0X%08x, but result is 0X%08x addr is 0x%0x\n",k, tmp_value_CHK,tmp_value_RST,(j+k));
          //else{
          //  printf("OK    line is %0d expect 0X%08x, result is 0X%08x addr is 0x%0x\n",k, tmp_value_CHK,tmp_value_RST,(j+k));
            ok++;
          
    }
       printf("OK num is %d\n",ok );
       npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr,  (int)Dst_Addr, 8 );       
     }
      if(ok==NPU_DATAOUT_DEPTH0){
       printf("Test OK..\n");
       }
      else
      {
        printf("Test not OK..\n");
      }
      return 0;
          
    
}

void ISR_ANN_INT (void){ 
  
  int a=0;
  int npu_on=0;
  //int delay = 200;
  a  = *(volatile int*)(ANN_REG_STATUS_END);

  clear_int();
  ICP |= (1<<16);
  
  printf(" finish_status is %d\n",a );
  //npu_on = *(volatile int*)(ANN_REG_STATUS_RUN);
  //printf(" npu_run status is %d\n",npu_on );
    
  if(a==1){    
     //  clear_int () ;
       uart_send("Now init_ram is complete\n",25);
       uart_wait_tx_done();
      // count++;
       npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr,  (int)Dst_Addr, 8 ); 
      // uart_send("Now npu start.\n",15);
      // uart_wait_tx_done();

      // npu_on = *(volatile int*)(ANN_REG_STATUS_RUN);
      // if(npu_on == 2){
      //   uart_send("npu running\n",12);
      //   uart_wait_tx_done();
      //  }

    //   Return_pro_params();
       
    }
    else if(a==2){ 
    //  clear_int () ;  
      uart_send("Now computing is complete\n",26);
      uart_wait_tx_done();
      count++; 
    }
} 
