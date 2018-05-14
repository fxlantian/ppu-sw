//2016/11/29 
//Lei

#include <stdio.h>
#include "memctl.h"
#include "camctl.h"
#include "int.h"
#include "event.h"
#include "gpio.h"

int main()
{
  int i;
  
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

  memctl_init();
  printf("DRAM init done!!!\n");

  camctl_init();
  printf("Camera init done!!!\n");

  int_enable();
  IER |= (1<< INT_CAM);	//enable camera interrupt
  printf("Enable global interrupt done!!!\n");

  camctl_start();

  for(i = 0; i < 50000; i = i+1) asm volatile("nop");

  int_disable();
  camctl_stop();
  printf("Camera stooooop!!!\n");

  return 0;
}
