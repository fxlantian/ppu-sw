#include "uart.h"
#include "string_lib.h"
#include "bar.h"
#include "gpio.h"
#include "stdio.h"
#include <stdint.h>
#include "ble.h"
#include <string.h>

#define BT_PIN_DEVICE_WAKE PIN10
#define BT_PIN_POWER PIN11
#define BT_PIN_UART_RTS PIN12
#define BT_PIN_UART_CTS PIN13

uint8_t data[10];

void delay_ms(volatile int delay)
{
    volatile int i;
    delay = delay * 2000;
    for(i = 0; i < delay; i++) asm volatile("nop");
}

int main()
{
  BOOTREG = 0x23000000;
  int ble_status = 0;
  uart_set_cfg(0, 107);
  uart1_set_cfg(0, 107);
  set_pin_function(PIN0,  FUNC_UART); 
  set_pin_function(PIN1,  FUNC_UART);

  uart_send("bluetooth init..\n", strlen("bluetooth init..\n"));
  //printf("bluetooth init..\n");
  ble_status = bluetooth_init();
  
  if(ble_status)
    uart_send("ble init success!\n", strlen("ble init success!\n"));
  else
    uart_send("ble init fail!\n", strlen("ble init fail!\n"));
  
  ble_status = bt_firmware_download(brcm_patchram_buf, brcm_patch_ram_length, brcm_patch_version);
  
  if(ble_status)
    uart_send("bt_firmware_download success!\n", strlen("bt_firmware_download success!\n"));
  else
    uart_send("bt_firmware_download fail!\n", strlen("bt_firmware_download fail!\n"));

}

int bluetooth_init(void)
{
  set_pin_function(BT_PIN_DEVICE_WAKE,  FUNC_GPIO); 
  set_gpio_pin_direction(BT_PIN_DEVICE_WAKE, DIR_OUT);
  set_gpio_pin_value(BT_PIN_DEVICE_WAKE, 0);
  delay_ms(100);

  set_pin_function(BT_PIN_POWER, FUNC_GPIO); 
  set_gpio_pin_direction(BT_PIN_POWER, DIR_OUT);
  set_gpio_pin_value(BT_PIN_POWER, 1);

  set_pin_function(BT_PIN_UART_RTS, FUNC_GPIO);
  set_gpio_pin_direction(BT_PIN_UART_RTS, DIR_OUT);
  set_gpio_pin_value(BT_PIN_UART_RTS, 1);

  set_pin_function(BT_PIN_UART_CTS, FUNC_GPIO);
  set_gpio_pin_direction(BT_PIN_UART_CTS, DIR_IN);
  
  uart1_set_cfg(0,107);

  delay_ms(500);
  set_gpio_pin_value(BT_PIN_UART_RTS, 0);

  /* Wait for Bluetooth chip to pull its RTS (host's CTS) low. 
  From observation using CRO, it takes the bluetooth chip > 170ms to pull its RTS low after CTS low */
  while (get_gpio_pin_value(BT_PIN_UART_CTS) == 1)
  {
        delay_ms(10);
  }
  return 1;
}

int bt_firmware_download(const uint8_t* firmware_image, uint32_t size, const char* version)
{
  uint8_t* data = (uint8_t*) firmware_image;
  uint32_t remaining_length = size;
  hci_event_extended_header_t hci_event;

  BT_BUS_IS_READY();
  uart_send("bt READY\n", strlen("bt READY\n"));
  ////printf("sizeof(hci_command_header_t):%d\n", sizeof(hci_command_header_t));
  bt_bus_transmit((const uint8_t* ) &hci_commands[ HCI_CMD_RESET ], sizeof(hci_command_header_t));
  bt_bus_receive((uint8_t* ) &hci_event, sizeof( hci_event ));
  VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_RESET ], sizeof( hci_event ) );
  //status = bt_host_update_baudrate(115200);
  //printf("bt reset done.\n");
  uart_send("bt reset done.\n", strlen("bt reset done.\n"));
  BT_BUS_IS_READY();
  bt_bus_transmit((const uint8_t* ) &hci_commands[ HCI_CMD_DOWNLOAD_MINIDRIVER ], sizeof(hci_command_header_t));
  bt_bus_receive((uint8_t* ) &hci_event, sizeof( hci_event ));
  VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_DOWNLOAD_MINIDRIVER ], sizeof( hci_event ) );
  uart_send("bt CMD_DOWNLOAD_MINIDRIVER done.\n", strlen("bt CMD_DOWNLOAD_MINIDRIVER done.\n"));
  //printf("bt CMD_DOWNLOAD_MINIDRIVER done.\n");
  /* The firmware image (.hcd format) contains a collection of hci_write_ram command + a block of the image,
  * followed by a hci_write_ram image at the end. Parse and send each individual command and wait for the response.
  * This is to ensure the integrity of the firmware image sent to the bluetooth chip.
  */
    while ( remaining_length )
    {
      uint32_t data_length = data[ 2 ] + 3; /* content of data length + 2 bytes of opcode and 1 byte of data length */
      hci_command_opcode_t command_opcode = *(uint16_t*) data;
      uint8_t temp_data[ 256 ];

      memset( &hci_event, 0, sizeof( hci_event ) );
      memset( temp_data, 0, sizeof( temp_data ) );

      /* 43438 requires the packet type before each write RAM command */
      temp_data[ 0 ] = HCI_COMMAND_PACKET;
      memcpy( &temp_data[ 1 ], data, data_length );
      //printf("data_length:%d\n",data_length);
      /* Send hci_write_ram command. The length of the data immediately follows the command opcode */
      bt_bus_transmit((const uint8_t* )temp_data, data_length + 1);
      bt_bus_receive((uint8_t* ) &hci_event, sizeof( hci_event ));
      //printf("command_opcode:%x\n",command_opcode);
      switch ( command_opcode )
      {
          case HCI_CMD_OPCODE_WRITE_RAM:
              //printf("HCI_CMD_OPCODE_WRITE_RAM\n");
              VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_WRITE_RAM ], sizeof( hci_event ) );

              /* Update remaining length and data pointer */
              data += data_length;
              remaining_length -= data_length;
              break;

          case HCI_CMD_OPCODE_LAUNCH_RAM:
              //printf("HCI_CMD_OPCODE_LAUNCH_RAM\n");
              VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_LAUNCH_RAM ], sizeof( hci_event ) );

              /* All responses have been read. Now let's flush residual data if any and reset remaining length */
              while((*((volatile int*)UART1_REG_LSR) & 0x1) == 0x1)
              {
                uart1_getchar();
              }

              remaining_length = 0;

              break;

          default:
              //printf("command_opcode error.\n");
              return 0;
      }
    }
    //printf("bt patch done.\n");
    BT_BUS_WAIT_UNTIL_READY();
    //printf("bt ready.\n");
    //send a break via uart
    *(volatile unsigned int*)(UART1_REG_LCR) |= 0x40;
    //printf("LCR:%x\n", *(volatile unsigned int*)(UART1_REG_LCR));
    uart_sendchar(*(volatile unsigned char*)(UART1_REG_LCR));
    //uart1_send(*(volatile unsigned int*)(UART1_REG_LCR), 1);
    delay_ms(1000);
    *(volatile unsigned int*)(UART1_REG_LCR) &= (~0x40);
    uart_sendchar(*(volatile unsigned char*)(UART1_REG_LCR));

    while((*((volatile int*)UART1_REG_LSR) & 0x1) == 0x1)
              {
                uart1_getchar();
              }

    bt_bus_transmit((const uint8_t* ) &hci_commands[ HCI_CMD_RESET ], sizeof(hci_command_header_t));
    bt_bus_receive((uint8_t* ) &hci_event, sizeof( hci_event ));
    VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_RESET ], sizeof( hci_event ) );
    uart_send("bt reset done.\n", strlen("bt reset done.\n"));
    return 1;
}

int bt_bus_is_ready( void )
{
  if(get_gpio_pin_value(BT_PIN_UART_CTS) == 1)
    return 0;
  else
    return 1;
}


int bt_host_update_baudrate( unsigned int newBaudRate )
{
  hci_event_extended_header_t hci_event;
  char update_command[ 10 ];
  int status = 1;

  update_command[ 0 ] = 0x01;
  update_command[ 1 ] = 0xFC;
  update_command[ 2 ] = 0x18;
  update_command[ 3 ] = 0x06;

  update_command[ 4 ] = 0x00; /* Encoded baud rate ; disable : 0 , enable : 1 */
  update_command[ 5 ] = 0x00; /* use Encoded form  ; disable : 0 , enable : 1 */

  /* issue BT hci update baudrate */
  update_command[ 6 ] = ( newBaudRate ) & 0xff;
  update_command[ 7 ] = ( newBaudRate >> 8 ) & 0xff;
  update_command[ 8 ] = ( newBaudRate >> 16 ) & 0xff;
  update_command[ 9 ] = ( newBaudRate >> 24 ) & 0xff;

  bt_bus_transmit((const uint8_t* )update_command, 10);
  bt_bus_receive( (uint8_t* ) &hci_event, sizeof( hci_event ));
  VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_UPDATE_BAUDRATE ], sizeof( hci_event ) );
  return 1;
}
void bt_bus_transmit(uint8_t* data_out, uint32_t size )
{
  for(uint32_t i = 0; i < size; i++){
    while(get_gpio_pin_value(BT_PIN_UART_CTS) == 1)
    {
    };
    uart1_sendchar(data_out[i]);
  }
}
void bt_bus_receive(uint8_t* data_in, uint32_t size)
{
  //printf("rx: ");
  uart_send("rx: ", strlen("rx: "));
  for(uint32_t i = 0; i < size; i++){
    data_in[i] = uart1_getchar();
    uart_sendchar(data_in[i]);
    //printf("%02x ", data_in[i]);
  }
  uart_send("rx done\n", strlen("rx done\n"));
  //printf("rx done\n");
}

/******************************************************
 *               Function Definitions
 ******************************************************/
int bt_issue_reset ( void )
{
    hci_event_extended_header_t hci_event;
    uint8_t hci_data[ 8 ] = { 0x00 };
    uint8_t hardware_error[ 4 ] = { 0x04, 0x10, 0x01, 0x00 };
    bt_bus_transmit( (const uint8_t* ) &hci_commands[ HCI_CMD_RESET ], sizeof(hci_command_header_t) );

    /* FIXME : This hardware parsing error should be handled by the firmware
     *  and will be eventually removed from this code*/

    /* if any hardware parsing error event just ignore it and read next bytes */
    bt_bus_receive( (uint8_t* ) hci_data, 4);
    if ( !memcmp( hardware_error, hci_data, 4 ) )
    {
        //printf( "hardware parsing error recieved \n" );
        bt_bus_receive( (uint8_t* ) &hci_event, sizeof( hci_event ));
        VERIFY_RESPONSE( &hci_event, &expected_hci_events[ HCI_CMD_RESET ], sizeof( hci_event ) );
        return 0;
    }
    else
    {
        bt_bus_receive( (uint8_t* ) &hci_data[4], 3); /* First reset command requires extra delay between write and read */
        VERIFY_RESPONSE( hci_data, &expected_hci_events[ HCI_CMD_RESET ], sizeof( hci_event ) );
        return 1;
    }
    
}