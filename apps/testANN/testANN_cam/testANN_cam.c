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
#include <bench.h>
#include <stdio.h>
#include "int.h"
#include "uart.h"
#include "event.h"
#include "../testANN.h"
#include "malloc.h"
#include "oled.h"
#include "bmp.h"
#include "camctl.h"
#include "i2c.h"
#include "camctl.h"
#include "sccb_bsp.h"
#include "gpio.h"
#include "memctl.h"
#include "spi.h"

#define ahb_pl1 0x28004000
#define DMA_BLOCK_INF 8
#define config_addr 0x22000000
#define data_addr   0x26000000
#define dst_addr   0x25000000

volatile int count = 0;
unsigned int* Dst_Addr;
volatile int Isr_Type;
unsigned char image_sampled[28*28];
unsigned char image_normal[28*28];
extern int dis_line;

void sample_image(volatile unsigned char* image_addr, unsigned char* image_sampled)
{
    u16 i,j;
    u16 k=0;      
    //image_addr->320*240 yuv 4:2:2
    volatile unsigned char* image_addr_ver = image_addr + 2 * 640;
    for(i=0;i<28;i++)
    {
      image_addr_ver = image_addr + i * 640 * 8 + 2 * 10; //pick a pixel every 8 line
      for(j=0;j<28;j++)
      {
        image_sampled[i*28 + j] = *image_addr_ver;
        image_addr_ver += 16;
      }
    }
}

void normalization_image(unsigned char* image_sampled)
{
    volatile int i,j,ver;
    unsigned char max=0;
    unsigned char min=255; 
    float temp[28*28];
    ver=0;
    /*for(i=0;i<28;i++)
    {
      for(j=0;j<28;j++)
      {
        temp[ver] = ((float)image_sampled[ver])/255;
        ver++;
        printf("temp:%f ",temp[ver]);
      }
    }*/
    
    
    for(i=0;i<28;i++)
    {
      for(j=0;j<28;j++)
      {
        if(max<image_sampled[ver])
          max = image_sampled[ver];
        if(min>image_sampled[ver])
          min = image_sampled[ver];
        ver++;
      }
    }
    ver=0;
    unsigned int max_dif = max - min;
    for(i=0;i<28;i++)
    {
      for(j=0;j<28;j++)
      {
        //image_sampled[ver] = ((max - image_sampled[ver]) * 255) / max_dif;
        if(image_sampled[ver]>90)
            image_sampled[ver] = 0;
        else
            image_sampled[ver] = 255;
        ver++;
      }
    }

    Address_set( 370, 100, 425, 155);
    OLED_DC_Set();
    unsigned char* image_addr_ver = image_sampled;
    unsigned char* image_addr_ver1 = image_sampled;
    for(i=0;i<28;i++)
    {
      for(j=0;j<28;j++)
      {
        LCD_WR_DATA8(*(image_addr_ver));
        LCD_WR_DATA8(*(image_addr_ver));
        LCD_WR_DATA8(*(image_addr_ver));

        LCD_WR_DATA8(*(image_addr_ver));
        LCD_WR_DATA8(*(image_addr_ver));
        LCD_WR_DATA8(*(image_addr_ver));
        image_addr_ver++;
      }

      for(j=0;j<28;j++)
      {
        LCD_WR_DATA8(*(image_addr_ver1));
        LCD_WR_DATA8(*(image_addr_ver1));
        LCD_WR_DATA8(*(image_addr_ver1));

        LCD_WR_DATA8(*(image_addr_ver1));
        LCD_WR_DATA8(*(image_addr_ver1));
        LCD_WR_DATA8(*(image_addr_ver1));
        image_addr_ver1++;
      }
    }
}

void qfix_image(volatile unsigned char* image_addr)
{
 
    int i,j;
    int ver = 0; 
    unsigned char* dst_image = (unsigned char*)data_addr;
     for(i=0;i<28;i++)
    {
      for(j=0;j<28;j++)
      {
          *(dst_image + ver) = image_addr[ver] >> 1;
          ver++;
      }
    }
}

void check_result(char* Dst_Addr)
{
    int test_i;
    signed char res=0;
    signed char big_last=0;
    signed char big_num=0;
    
    for(test_i=0;test_i<10;test_i++)
    {
      res = *(Dst_Addr + test_i);
      //printf("%02x ",res);

      if(res > big_last){
        big_num = test_i;
        big_last = res;
      }
    }

    //printf("possible_num:%x\n",big_num);
    if(big_num==0)
     LCD_ShowString(370 ,170 ,"NUMBER: 0");
    else if(big_num==1)
     LCD_ShowString(370 ,170 ,"NUMBER: 1");
   else if(big_num==2)
     LCD_ShowString(370 ,170 ,"NUMBER: 2");
   else if(big_num==3)
     LCD_ShowString(370 ,170 ,"NUMBER: 3");
   else if(big_num==4)
     LCD_ShowString(370 ,170 ,"NUMBER: 4");
   else if(big_num==5)
     LCD_ShowString(370 ,170 ,"NUMBER: 5");
   else if(big_num==6)
     LCD_ShowString(370 ,170 ,"NUMBER: 6");
   else if(big_num==7)
     LCD_ShowString(370 ,170 ,"NUMBER: 7");
   else if(big_num==8)
     LCD_ShowString(370 ,170 ,"NUMBER: 8");
   else if(big_num==9)
     LCD_ShowString(370 ,170 ,"NUMBER: 9");
}

int main() {
    
    BOOTREG = 0x00000000;
    memctl_init();
    set_pin_function(PIN0, FUNC_I2C);
    set_pin_function(PIN1, FUNC_I2C);

    set_pin_function(PIN4, FUNC_CAM);
    set_pin_function(PIN5, FUNC_CAM);
    set_pin_function(PIN6, FUNC_CAM);
    set_pin_function(PIN7, FUNC_CAM);
    set_pin_function(PIN8, FUNC_CAM);
    set_pin_function(PIN9, FUNC_CAM);
    set_pin_function(PIN10, FUNC_CAM);
    set_pin_function(PIN11, FUNC_CAM);
    set_pin_function(PIN12, FUNC_CAM);
    set_pin_function(PIN13, FUNC_CAM);

    set_pin_function(PIN2, FUNC_SPI);//APIM1SDO
    set_pin_function(PIN3, FUNC_GPIO);//DC

    uart_set_cfg(0,49);
    uart_send("start..\n",8);
    uart_wait_tx_done();

    //set ann master a high priority
    int pvalue=0;
    *(volatile int*) (ahb_pl1)  = 10;
    pvalue = *(volatile int*)(ahb_pl1);
    printf("master 1 pvalue is %d\n", pvalue);

    //move ann config data to mem boundary
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

    //Dst_Addr = (unsigned int*)malloc(sizeof(int) * NPU_DATAOUT_DEPTH0);
    Dst_Addr = (unsigned int*)dst_addr;
    printf("allocate dst_addr:%x\n",(unsigned int)Dst_Addr);
    
    //prepare lcd and cam
	  Lcd_Init();
    LCD_Clear(BLACK);
    BACK_COLOR=BLACK;
    POINT_COLOR=WHITE;
    display_shade(110);
    //display_image(5, 100, 105 - 1, 200 - 1, (volatile unsigned char*)logo);
    printf_lcd("ICT WuSystem!");
    POINT_COLOR = GREEN;
    printf_lcd("      U");
    printf_lcd("  UUUUUUUU       UUUUUUUU    UUU    UUU");
    printf_lcd(" U    U   U       U      U    U      U");
    printf_lcd("U     U    U      U      U    U      U");
    printf_lcd("U     U    U      U     U     U      U");
    printf_lcd(" UU   U   UU      U Chip      U      U");
    printf_lcd("   UUUUUUU        U     5G    U      U");
    printf_lcd("      U   RISC-V  U      AI   U      U");
    printf_lcd("      UU         UUUUUUU  IOT  UUUUUU");
    POINT_COLOR=WHITE;
    //while(1){
        LCD_ShowString(150,dis_line ,"-> -");
        delay_ms(300);
        LCD_ShowString(150,dis_line ,"-> \\");
        delay_ms(300);
        LCD_ShowString(150,dis_line ,"-> |");
        delay_ms(300);
        LCD_ShowString(150,dis_line ,"-> /");
        delay_ms(300);
 // }
    LCD_Clear(BLACK);
    LCD_DrawRectangle(369, 99, 426, 156);
    POINT_COLOR = RED;
    LCD_ShowString(350 ,40 ,"ICT PPU Demo");
    POINT_COLOR = WHITE;
    LCD_ShowString(350 ,60 ,"Enable Edge AI");

    int ver;
    SCCB_init();
    SCCB_WriteByte(0x12, 0x80);
    SCCB_WriteByte(0x11, 0x01); //2/ pre-scale
    SCCB_WriteByte(0x0d, 0x00); //PLL 0x
    //SCCB_WriteByte(0x12, 0x06); //VGA
    SCCB_WriteByte(0x12, 0x46); //QVGA,RGB
   /* do{
      i2c_send_data(0x42); 
      i2c_send_command(I2C_START_WRITE);
    }while(!i2c_get_ack());
      
    i2c_send_command(I2C_STOP);
    while(i2c_busy());*/
    ver = SCCB_ReadByte(0x12);
    printf("SCCB receive: %x\n", ver);
    printf("SCCB receive: %x\n", ver);
    printf("SCCB receive: %x\n", ver);
    printf("SCCB receive: %x\n", ver);
    ver = SCCB_ReadByte(0x0d);
    printf("SCCB receive: %x\n", ver);

    Isr_Type = 0;
    camctl_init();
    int_enable();
    IER |= (1<<16);
    init_pro(IM_DEPTH0, WEIGTH_DEPTH0, BIAS_DEPTH0, config_addr, DMA_BLOCK_INF);   
     
    while(!count);
    count = 0;
    printf("ANN init done!!!\n");
    camctl_start();

    while(1){
       if(Isr_Type != 0)
        {
           //printf("Get in ISR, Service Type: %d\n", Isr_Type);
           //Address_set(0,40,320-1,280-1);
           
           if(Isr_Type == 1) {
                Address_set(1,40,320,280-1);
                OLED_DC_Set();
                spi1_setup_cmd_addr(0, 24, 0, 0);
                display_cam((volatile unsigned char*)FRAME1_ADDR);
                sample_image((volatile unsigned char*)FRAME1_ADDR,image_sampled);
                normalization_image(image_sampled);
                qfix_image(image_sampled);
                npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr, (int)Dst_Addr, DMA_BLOCK_INF ); 
                while(count!=2) {
                  }
                count=0;
                check_result((char*)Dst_Addr);
               //send_img((volatile unsigned char*)FRAME1_ADDR, IMAGE_LENGTH*2);
            }
            else if(Isr_Type == 2){
                Address_set(1,40,320,280-1);
                OLED_DC_Set();
                spi1_setup_cmd_addr(0, 24, 0, 0);
                display_cam((volatile unsigned char*)FRAME2_ADDR);
                sample_image((volatile unsigned char*)FRAME2_ADDR,image_sampled);
                normalization_image(image_sampled);
                qfix_image(image_sampled);
                npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr, (int)Dst_Addr, DMA_BLOCK_INF ); 
                while(count!=2) {
                  }
                count=0;
                check_result((char*)Dst_Addr);
                //send_img((volatile unsigned char*)FRAME2_ADDR, IMAGE_LENGTH*2);
            }
            else if(Isr_Type == 3){
                Address_set(11,40,330,280-1);
                OLED_DC_Set();
                spi1_setup_cmd_addr(0, 24, 0, 0);
                display_cam((volatile unsigned char*)FRAME3_ADDR);
                sample_image((volatile unsigned char*)FRAME3_ADDR,image_sampled);
                normalization_image(image_sampled);
                qfix_image(image_sampled);
                npu_pro(NPU_DATAIN_DEPTH0, NPU_DATAOUT_DEPTH0, data_addr, (int)Dst_Addr, DMA_BLOCK_INF ); 
                while(count!=2) {
                  }
                count=0;
                check_result((char*)Dst_Addr);
                //send_img((volatile unsigned char*)FRAME3_ADDR, IMAGE_LENGTH*2);
            }
        Isr_Type = 0;
        camctl_int_enable(SET_FM1_INT, SET_FM2_INT, SET_FM3_INT, UNSET_RQFUL_INT, UNSET_RQOVF_INT, SET_PROERR_INT);
       // printf("Status reg1:%d\n", *(volatile int*) (CAMCTL_STATUS));
        camctl_start(); //open the camera
        }
    }
  return 0;   
  
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
    
  if(a==1){		 //init_ram is complete
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
void ISR_CAM(void)
{
  int val;
  
  //uart_send("Enter ISP\n", 10);
  //uart_wait_tx_done();

  camctl_stop();  //close the camera
  ICP = (1 << INT_CAM);
  //IER &= ~(1<< INT_CAM);
 // camctl_int_disable(UNSET_FM1_INT, UNSET_FM2_INT, UNSET_FM3_INT, UNSET_RQFUL_INT, UNSET_RQOVF_INT, UNSET_PROERR_INT);

  val = *(volatile int*) (CAMCTL_STATUS);

  if(DATAOK_FRAME1(val) == 1) {
    Isr_Type = 1;
    val = CLR_FRAME1_INT(val);
  }

  if(DATAOK_FRAME2(val) == 1) {
    Isr_Type = 2;
    val = CLR_FRAME2_INT(val);
  }

  if(DATAOK_FRAME3(val) == 1) {
    Isr_Type = 3;
    val = CLR_FRAME3_INT(val);
  }

  if(RQ_FUL(val) == 1) {
    Isr_Type = 4;
    val = CLR_RQFUL_INT(val);
  }

  if(RQ_OVF(val) == 1) {
    Isr_Type = 5;
    val = CLR_RQOVF_INT(val);
  }

  if(PROTOCAL_ERR0(val) == 1) {
    Isr_Type = 6;
    val = CLR_PROERR0_INT(val);
  }

  if(PROTOCAL_ERR1(val) == 1) {
    Isr_Type = 7;
    val = CLR_PROERR1_INT(val);
  }
  ///printf("val:%d\n", val);
  *(volatile int*) (CAMCTL_STATUS) = val;
  //printf("Status reg:%d\n", *(volatile int*) (CAMCTL_STATUS));
  //camctl_int_enable(SET_FM1_INT, SET_FM2_INT, SET_FM3_INT, UNSET_RQFUL_INT, UNSET_RQOVF_INT, SET_PROERR_INT);

}

//capture code
/*
    int_enable();
    IER |= (1<< INT_CAM); //enable camera interrupt
    EER |= (1<< INT_CAM); //enable camera interrupt
    
    int label=0;
    int k=0;
    for(int c=0; c<50; c++)
    {
      if(label==0)
         printf_lcd("label: 0");
       else if(label==1)
         printf_lcd("label: 1");
      else if(label==2)
          printf_lcd("label: 2");
       else if(label==3)
           printf_lcd("label: 3");
      else if(label==4)
           printf_lcd("label: 4");
      else if(label==5)
           printf_lcd("label: 5");
       else if(label==6)
            printf_lcd("label: 6");
       else if(label==7)
          printf_lcd("label: 7");
      else if(label==8)
          printf_lcd("label: 8");
      else if(label==9)
          printf_lcd("label: 9");
      camctl_start(); //open the camera
      while(1){
       if(Isr_Type != 0)
        {
           //printf("Get in ISR, Service Type: %d\n", Isr_Type);
           if(Isr_Type == 1) {
                sample_image((volatile unsigned char*)FRAME1_ADDR,image_sampled);
            }
            else if(Isr_Type == 2){
                sample_image((volatile unsigned char*)FRAME2_ADDR,image_sampled);
            }
            else if(Isr_Type == 3){
                sample_image((volatile unsigned char*)FRAME3_ADDR,image_sampled);
            }

        gpioi = get_gpio_pin_value(8);
        if(!gpioi)
        {
          k=0;
          printf("%d %d\n", label, c - label*5);
          for(int i=0;i<28;i++)
          {
            for(int j=0;j<28;j++)
            {
              printf("%d ", image_sampled[k]);
              k++;
            }
          }
          printf("\n");
          while(!get_gpio_pin_value(8));
          Isr_Type = 0;
          camctl_int_enable(SET_FM1_INT, SET_FM2_INT, SET_FM3_INT, UNSET_RQFUL_INT, UNSET_RQOVF_INT, SET_PROERR_INT);
          // printf("Status reg1:%d\n", *(volatile int*) (CAMCTL_STATUS));
          camctl_start(); //open the camera
          break;
        }
        Isr_Type = 0;
        camctl_int_enable(SET_FM1_INT, SET_FM2_INT, SET_FM3_INT, UNSET_RQFUL_INT, UNSET_RQOVF_INT, SET_PROERR_INT);
        // printf("Status reg1:%d\n", *(volatile int*) (CAMCTL_STATUS));
        camctl_start(); //open the camera
        }
      }
      if((c + 1)%5 == 0)
        label++;
    }*/