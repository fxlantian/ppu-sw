#include "../testANN.h"
#include <stdio.h>
#include "gpio.h"
#include "ann.h"
#include "int.h"
#include "event.h"
#include "uart.h"
#include "memctl.h"

#define ahb_pl1 0x28004000
#define DMA_BLOCK_INF 8
#define config_addr 0x23000000
#define data_addr   0x26000000
#define dst_addr   0x25000000

volatile int count = 0;
unsigned int* Dst_Addr;

int main(){
  BOOTREG = 0x00000000;
    memctl_init();
	 uart_set_cfg(0,49);
    uart_send("start..\n",8);
    uart_wait_tx_done();

    set_pin_function(PIN3, FUNC_GPIO);//DC
    set_gpio_pin_direction(PIN3, DIR_OUT);
    set_gpio_pin_value(PIN3, 0);

    //move ann config data to mem boundary
    int move_num;
    int *move_ptr = (int*)config_addr;
    move_num = IM_DEPTH0 + WEIGTH_DEPTH0  + BIAS_DEPTH0 / 4 + 128;
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
    printf("moving data array number: %d ..\n", NPU_DATAIN_DEPTH0);
    for (int i = 0;i < NPU_DATAIN_DEPTH0; i++)
    {
      *(move_ptr + i) = data_input[i];
    }
    for (int i = 0; i < NPU_DATAIN_DEPTH0; i++)
    {
      if(*(move_ptr + i) != data_input[i]){
        printf("move data error at %d\n",i);
        while(1);
      }
      //printf("0x%08x,\n", *(move_ptr + i));
    }

        //set ann master a high priority
    int pvalue=0;
    *(volatile int*) (ahb_pl1)  = 10;
    pvalue = *(volatile int*)(ahb_pl1);
    printf("master 1 pvalue is %d\n", pvalue);

    //Dst_Addr = (unsigned int*)malloc(sizeof(int) * NPU_DATAOUT_DEPTH0);
    Dst_Addr = (unsigned int*)dst_addr;
    printf("allocate dst_addr:%x\n",(unsigned int)Dst_Addr);
    
    printf("CGREG:%x\n",CGREG);
    //CGREG = 0x802|(1<<CGEVENT)|(1<<CGGPIO);
    CGREG = 0x802|(1<<CGEVENT);
    printf("CGREG:%x\n",CGREG);

	int_enable();
    IER |= (1<<16);
    init_pro(IM_DEPTH0, WEIGTH_DEPTH0, BIAS_DEPTH0, config_addr, DMA_BLOCK_INF);   
     
    while(!count);
    count = 0;
    printf("ANN init done!!!\n");

    int ver=0;

	while(1){
		if(ver)
			ver=0;
		else
			ver=1;

		set_gpio_pin_value(PIN3, ver);
		//npu_pro(test_data[0], Dst_Addr, INPUT_NUM, HIDDEN_NUM0, HIDDEN_NUM1, OUTPUT_NUM, weights);
		npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr, (int)Dst_Addr, DMA_BLOCK_INF ); 
		while(count!=2) {
                  }
                count=0;
      /*for(int j = 0; j != NPU_DATAOUT_DEPTH0; j++){
    		if(*(Dst_Addr+j)!=data_check[j])
      			printf("wrong!!calc:%x expect:%x\n", *(Dst_Addr+j),data_check[j]);
    		else
      			printf("calc:%x expect:%x\n", *(Dst_Addr+j),data_check[j]);
      	}*/
        
	}
}

void ISR_ANN_INT (void){ 
  
  int a=0;
  int npu_on=0;
  //int delay = 200;
  a = *(volatile int*)(ANN_REG_STATUS_END);

  clear_int();
  ICP |= (1<<16);
  
  //printf(" finish_status is %d\n",a );
  //npu_on = *(volatile int*)(ANN_REG_STATUS_RUN);
  //printf(" npu_run status is %d\n",npu_on );
    
  if(a==1){    //init_ram is complete
      IER |= (1<< INT_CAM); //enable camera interrupt
      EER |= (1<< INT_CAM); //enable camera interrupt
      count++;
       //npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, (int)&data_input,  (int)Dst_Addr, NPU_DMA_BLOCK_INFO0 );    
      // uart_send("Now npu start.\n",15);
      // uart_wait_tx_done();

      // npu_on = *(volatile int*)(ANN_REG_STATUS_RUN);
      // if(npu_on == 2){
      //   uart_send("npu running\n",12);
      //   uart_wait_tx_done();
      //  }

    //   Return_pro_params();
       
    }
    else if(a==2){ //computing is complete
      count=2; 
    }
} 