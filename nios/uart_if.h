//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  uart_if.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants for uart_if.c
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_UART_IF__H
#define __LAB_7_UART_IF__H

#include "nios_std_types.h"   // standard data types
#include "codebreaker.h"      // for CB_COLOR_LENGTH

// JTAG UART register offsets and masks
#define JTAG_DATA_REG_OFFSET        0
#define JTAG_CTRL_REG_OFFSET        1
#define JTAG_UART_WSPACE_MASK       0xFFFF0000
#define JTAG_UART_RV_BIT_MASK       0x00008000
#define JTAG_UART_DATA_MASK         0x000000FF
#define JTAG_UART_RIRQ_EN_MASK      0x00000001
#define JTAG_UART_RIRQ_PEND_MASK    0x00000100

// Constants
#define INIT_MESSAGE_0  "Game System JTAG UART driver is active.\n"
#define INIT_MESSAGE_1  "This Game System has Super Cow Powers.\n"

#define UART_RECVBUFFER (CB_COLOR_LENGTH + 1)

#define UART_MAINMODE   0x1
#define UART_GAMEMODE   0x2

// Prototypes for public functions
void uart_SendByte(uint8 byte);
void uart_SendString(uint8 *msg);
uint32 uart_RecvString(uint8 *str);
void uart_SetMode(uint32 mode);
void uart_init();

#endif /* __LAB_7_UART_IF__H */
