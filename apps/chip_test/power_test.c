// Copyright 2015 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "utils.h"
#include "string_lib.h"
#include "bar.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "string_lib.h"
#include "bar.h"
#include "gpio.h"
#include "stdio.h"
#include <stdint.h>
#include <string.h>

volatile int ver; 
int main()
{
  volatile int i;
  volatile int a;
  memctl_init();

  set_pin_function(PIN9,  FUNC_GPIO); //gpio2
  set_pin_function(PIN10, FUNC_GPIO); //gpio3
  set_pin_function(PIN11, FUNC_GPIO); //gpio4
  set_pin_function(PIN12, FUNC_GPIO); //gpio5
  set_pin_function(PIN13, FUNC_GPIO); //gpio6

  set_gpio_pin_direction(PIN9, DIR_OUT);//gpio2
  set_gpio_pin_direction(PIN10, DIR_OUT);//gpio3
  set_gpio_pin_direction(PIN11, DIR_OUT);//gpio4
  set_gpio_pin_direction(PIN12, DIR_OUT);//gpio5
  set_gpio_pin_direction(PIN13, DIR_OUT);//gpio6

  printf("Successfully set GPIO OUTPUT\n");
  while(1){
      for (int i = 0; i < 0xfffff; i++)
           a = a * 2;

      set_gpio_pin_value(PIN9, 0);
      set_gpio_pin_value(PIN10, 0);
      set_gpio_pin_value(PIN11, 0);
      set_gpio_pin_value(PIN12, 0);
      set_gpio_pin_value(PIN13, 0);

      switch(ver % 5)
      {
        case 0: set_gpio_pin_value(PIN9, 1);
          printf("Ver:%d, Mod:%d, 0\n",ver,ver % 5);
          break;
        case 1: set_gpio_pin_value(PIN10, 1);
          printf("Ver:%d, Mod:%d, 1\n",ver,ver % 5);
          break;
        case 2: set_gpio_pin_value(PIN11, 1);
          printf("Ver:%d, Mod:%d, 2\n",ver,ver % 5);
          break;
        case 3: set_gpio_pin_value(PIN12, 1);
          printf("Ver:%d, Mod:%d, 3\n",ver,ver % 5);
          break;
        case 4: set_gpio_pin_value(PIN13, 1);
          printf("Ver:%d, Mod:%d, 4\n",ver,ver % 5);
          break;
        default: 
          set_gpio_pin_value(PIN9, 1);
          set_gpio_pin_value(PIN10, 1);
          set_gpio_pin_value(PIN11, 1);
          set_gpio_pin_value(PIN12, 1);
          set_gpio_pin_value(PIN13, 1);
          printf("Can't happen event!!--Ver:%d, Mod:%d\n, d",ver,ver % 5);
          break;
      }
      ver++;

    }


  printf("Done!!!\n");

  return 0;
}
