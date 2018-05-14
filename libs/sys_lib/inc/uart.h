// Copyright 2016 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

/**
 * @file
 * @brief 16750 UART library.
 *
 * Provides UART helper function like setting
 * control registers and reading/writing over
 * the serial interface.
 *
 */
#ifndef _UART_H
#define _UART_H

#include "ppu.h"
#include <stdint.h>
/*
 -- LCR register signals
    signal iLCR_WLS         : std_logic_vector(1 downto 0);     -- LCR: Word length select
    signal iLCR_STB         : std_logic;                        -- LCR: Number of stop bits
    signal iLCR_PEN         : std_logic;                        -- LCR: Parity enable
    signal iLCR_EPS         : std_logic;                        -- LCR: Even parity select
    signal iLCR_SP          : std_logic;                        -- LCR: Sticky parity
    signal iLCR_BC          : std_logic;                        -- LCR: Break control
    signal iLCR_DLAB        : std_logic;                        -- LCR: Divisor latch access bit
*/
#define UART_REG_RBR ( UART_BASE_ADDR + 0x00) // Receiver Buffer Register (Read Only)
#define UART_REG_DLL ( UART_BASE_ADDR + 0x00) // Divisor Latch (LS)
#define UART_REG_THR ( UART_BASE_ADDR + 0x00) // Transmitter Holding Register (Write Only)
#define UART_REG_DLM ( UART_BASE_ADDR + 0x04) // Divisor Latch (MS)
#define UART_REG_IER ( UART_BASE_ADDR + 0x04) // Interrupt Enable Register
#define UART_REG_IIR ( UART_BASE_ADDR + 0x08) // Interrupt Identity Register (Read Only)
#define UART_REG_FCR ( UART_BASE_ADDR + 0x08) // FIFO Control Register (Write Only)
#define UART_REG_LCR ( UART_BASE_ADDR + 0x0C) // Line Control Register
#define UART_REG_MCR ( UART_BASE_ADDR + 0x10) // MODEM Control Register
#define UART_REG_LSR ( UART_BASE_ADDR + 0x14) // Line Status Register
#define UART_REG_MSR ( UART_BASE_ADDR + 0x18) // MODEM Status Register
#define UART_REG_SCR ( UART_BASE_ADDR + 0x1C) // Scratch Register

#define UART1_REG_RBR ( UART1_BASE_ADDR + 0x00) // Receiver Buffer Register (Read Only)
#define UART1_REG_DLL ( UART1_BASE_ADDR + 0x00) // Divisor Latch (LS)
#define UART1_REG_THR ( UART1_BASE_ADDR + 0x00) // Transmitter Holding Register (Write Only)
#define UART1_REG_DLM ( UART1_BASE_ADDR + 0x04) // Divisor Latch (MS)
#define UART1_REG_IER ( UART1_BASE_ADDR + 0x04) // Interrupt Enable Register
#define UART1_REG_IIR ( UART1_BASE_ADDR + 0x08) // Interrupt Identity Register (Read Only)
#define UART1_REG_FCR ( UART1_BASE_ADDR + 0x08) // FIFO Control Register (Write Only)
#define UART1_REG_LCR ( UART1_BASE_ADDR + 0x0C) // Line Control Register
#define UART1_REG_MCR ( UART1_BASE_ADDR + 0x10) // MODEM Control Register
#define UART1_REG_LSR ( UART1_BASE_ADDR + 0x14) // Line Status Register
#define UART1_REG_MSR ( UART1_BASE_ADDR + 0x18) // MODEM Status Register
#define UART1_REG_SCR ( UART1_BASE_ADDR + 0x1C) // Scratch Register

#define RBR_UART REGP_8(UART_REG_RBR) 
#define DLL_UART REGP_8(UART_REG_DLL) 
#define THR_UART REGP_8(UART_REG_THR)
#define DLM_UART REGP_8(UART_REG_DLM)
#define IER_UART REGP_8(UART_REG_IER) 
#define IIR_UART REGP_8(UART_REG_IIR) 
#define FCR_UART REGP_8(UART_REG_FCR) 
#define LCR_UART REGP_8(UART_REG_LCR) 
#define MCR_UART REGP_8(UART_REG_MCR) 
#define LSR_UART REGP_8(UART_REG_LSR) 
#define MSR_UART REGP_8(UART_REG_MSR) 
#define SCR_UART REGP_8(UART_REG_SCR) 

#define RBR_UART1 REGP_8(UART1_REG_RBR) 
#define DLL_UART1 REGP_8(UART1_REG_DLL) 
#define THR_UART1 REGP_8(UART1_REG_THR)
#define DLM_UART1 REGP_8(UART1_REG_DLM)
#define IER_UART1 REGP_8(UART1_REG_IER) 
#define IIR_UART1 REGP_8(UART1_REG_IIR) 
#define FCR_UART1 REGP_8(UART1_REG_FCR) 
#define LCR_UART1 REGP_8(UART1_REG_LCR) 
#define MCR_UART1 REGP_8(UART1_REG_MCR) 
#define LSR_UART1 REGP_8(UART1_REG_LSR) 
#define MSR_UART1 REGP_8(UART1_REG_MSR) 
#define SCR_UART1 REGP_8(UART1_REG_SCR) 


#define DLAB 1<<7 	//DLAB bit in LCR reg
#define ERBFI 1 	//ERBFI bit in IER reg
#define ETBEI 1<<1 	//ETBEI bit in IER reg
#define PE 1<<2 	//PE bit in LSR reg
#define THRE 1<<5 	//THRE bit in LSR reg
#define DR 1	 	//DR bit in LSR reg


#define UART_FIFO_DEPTH 64

//UART_FIFO_DEPTH but to be compatible with Arduino_libs and also if in future designs it differed
#define SERIAL_RX_BUFFER_SIZE UART_FIFO_DEPTH 	
#define SERIAL_TX_BUFFER_SIZE UART_FIFO_DEPTH

void uart_set_cfg(int parity, uint16_t clk_counter);

void uart_send(const char* str, unsigned int len);
void uart_sendchar(const char c);

char uart_getchar();

void uart_wait_tx_done(void);

void uart1_set_cfg(int parity, uint16_t clk_counter);

void uart1_send(const unsigned char* str, unsigned int len);
void uart1_sendchar(const unsigned char c);

char uart1_getchar();

void uart1_wait_tx_done(void);
#endif
