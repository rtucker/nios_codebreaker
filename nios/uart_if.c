//***************************  C Source Code  *****************************
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Homework 7: Game System
//
//      FILE NAME:  uart_if.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//    This design implements a few useful functions for interacting with
//    the JTAG UART on an Altera DE2 board, with interrupts for handling
//    received data.
//
//*************************************************************************
//*************************************************************************

#include <sys/alt_irq.h>            // for IRQ support functions
#include <string.h>                 // string handling
#include "nios_std_types.h"         // for standard embedded types
#include "system.h"                 // for Qsys defines
#include "uart_if.h"                // uart_if headers
#include "utilities.h"

// pointers to registers
volatile uint32* uartDataRegPtr = ((uint32*)JTAG_UART_0_BASE +
                                            JTAG_DATA_REG_OFFSET);
volatile uint32* uartCtrlRegPtr = ((uint32*)JTAG_UART_0_BASE +
                                            JTAG_CTRL_REG_OFFSET);

// place to store incoming bytes
uint8  _recvstr_data[UART_RECVBUFFER];
uint32 _recvstr_ready;      // TRUE when there's a string ready
uint32 _recvstr_idx;        // used as index for _recvstr_data

// flag to set what characters we'll accept
uint32 _uart_mode;

//-------------------------------------------------------------------------
// NAME:        _uart_recv_isr
//
// DESCRIPTION: Interrupt service routine for servicing incoming data from
//              the UART.
//-------------------------------------------------------------------------
void _uart_recv_isr(void *context)
{
  uint32  data;
  uint8   character;

  if (0 != (*uartCtrlRegPtr & JTAG_UART_RIRQ_PEND_MASK))
  {
    // It's a valid interrupt: fetch the data register and test rvalid
    data = *uartDataRegPtr;
    if (0 != (data & JTAG_UART_RV_BIT_MASK))
    {
      character = (uint8)(data & 0xFF);
      if ((character >= 'a') && (character <= 'z'))
      {
        // convert lower-case characters to upper case
        character -= 0x20;
      } /* if */
      // Start testing for character validity
      if (_recvstr_ready)
      {
        // Can't do much until this is cleared...
        uart_SendString((uint8*)"uart_recv_isr: can't process character until previous line is picked up\n");
      } /* if */
      else if (('\b' == character) && (_recvstr_idx > 0))
      {
        // backspace: echo it, and decrement our index
        uart_SendByte(character);
        _recvstr_data[_recvstr_idx--] = NULL;
      } /* if */
      else if ('\n' == character)
      {
        // newline character: send stuff out
        _recvstr_data[_recvstr_idx] = NULL;
        _recvstr_ready = TRUE;
        _recvstr_idx = 0;
      } /* else if */
      else if (_recvstr_idx+1 < UART_RECVBUFFER)
      {
        // We have a character and a place to put it.
        if (UART_MAINMODE == _uart_mode)
        {
          // Main mode: allow normal characters plus backspace and newline
          if ((character >= 0x20) && (character <= 0x7E))
          {
            uart_SendByte(character);
            _recvstr_data[_recvstr_idx++] = character;
          } /* if */
        } /* if */
        else if (UART_GAMEMODE == _uart_mode)
        {
          // Game mode: only accept colors
          switch (character)
          {
            case 'G':
            case 'B':
            case 'R':
            case 'O':
            case 'Y':
            case 'W':
              uart_SendByte(character);
              _recvstr_data[_recvstr_idx++] = character;
              break;
          } /* switch */
        } /* else if */
      } /* else if */
    } /* if */
  } /* if */
  else
  {
    // probable error condition
    uart_SendString((uint8*)"uart_recv_isr: got interrupt but nothing to receive??\n");
  } /* else */

  return;
} /* _uart_recv_isr */

//-------------------------------------------------------------------------
// NAME:        uart_SendByte
//
// DESCRIPTION: Sends a byte to the UART.  Blocks until there is room to
//              write the byte into the buffer.
// ARGUMENTS:   uint8 byte, a single character to send
// RETURNS:     void
//-------------------------------------------------------------------------
void uart_SendByte(uint8 byte)
{
  while (0 == (*uartCtrlRegPtr & JTAG_UART_WSPACE_MASK))
  {
    // available write space is zero.
    // spin until the FIFO has room.
  } /* while */

  // store our byte to the FIFO
  *uartDataRegPtr = (uint32)byte;
} /* uart_SendByte */

//-------------------------------------------------------------------------
// NAME:        uart_SendString
//
// DESCRIPTION: Sends a NULL-terminated string to the UART.  This will
//              block if uart_SendByte blocks.
// ARGUMENTS:   uint8* msg, a pointer to a null-terminated string
// RETURNS:     void
//-------------------------------------------------------------------------
void uart_SendString(uint8 *msg)
{
  uint8 byte;

  while (TRUE)
  {
    byte = *msg;          // fetch next character from the string
    if (NULL == byte)
    {
      break;              // we're at the end of the string
    } /* if end */
    uart_SendByte(byte);  // send to the UART
    msg += 1;             // re-point at the next character
  } /* while */
} /* uart_SendString */

//-------------------------------------------------------------------------
// NAME:        uart_RecvString
//
// DESCRIPTION: Returns a string of inbound data if available
// ARGUMENTS:   uint8[], an array to copy things to
// RETURNS:     uint32, TRUE if string was delivered, FALSE otherwise
//-------------------------------------------------------------------------
uint32 uart_RecvString(uint8 str[])
{
  uint32 result = FALSE;

  int irq_enabled;

  // There is a possible race condition here, so disable the interrupt
  // for the JTAG UART while we're doing this.
  //irq_enabled = alt_ic_irq_enabled(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,
  //                                 JTAG_UART_0_IRQ);
  irq_enabled = FALSE;      // XXX: troubleshooting
  if (irq_enabled)
  {
    alt_ic_irq_disable(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,
                       JTAG_UART_0_IRQ);
  } /* if */

  // Check to see if we have something to return.  If so, update the ptr
  if (_recvstr_ready)
  {
    strcpy(str, _recvstr_data);
    _recvstr_ready = FALSE;
    result = TRUE;
  } /* if */

  // Re-enable the JTAG UART interrupt now that we're done.
  if (irq_enabled)
  {
    alt_ic_irq_enable(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,
                      JTAG_UART_0_IRQ);
  } /* if */

  // Done -- return the result to the caller
  return result;
} /* uart_RecvString */

//-------------------------------------------------------------------------
// NAME:        uart_SetMode
//
// DESCRIPTION: Sets the mode for which characters the UART driver will
//              accept.
// ARGUMENTS:   uint32 mode, which should be one of UART_MAINMODE or
//                           UART_GAMEMODE.
// RETURNS:     void
//-------------------------------------------------------------------------
void uart_SetMode(uint32 mode)
{
  _uart_mode = mode;
  return;
} /* uart_SetMode */

//-------------------------------------------------------------------------
// NAME:        uart_init
//
// DESCRIPTION: Sets up UART, registers interrupt service routine, and
//              sends initial text to the UART.
// ARGUMENTS:   None
// RETURNS:     void
//-------------------------------------------------------------------------
void uart_init()
{
  uint8* test_msg_0 = (uint8*)INIT_MESSAGE_0;
  uint8* test_msg_1 = (uint8*)INIT_MESSAGE_1;
  uint8 byte;

  // Send first welcome message
  uart_SendString(test_msg_0);

  // initialize our flags and buffers
  _uart_mode = UART_MAINMODE;
  memset(_recvstr_data, 0, sizeof(_recvstr_data));
  _recvstr_ready = FALSE;
  _recvstr_idx = 0;

  // read the data reg, to clear it
  byte = *uartDataRegPtr;

  // register our ISR.
  alt_ic_isr_register(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,
                      JTAG_UART_0_IRQ,
                      _uart_recv_isr, 0, 0);

  // enable UART read interrupt, disable write interrupt
  *uartCtrlRegPtr = JTAG_UART_RIRQ_EN_MASK;

  // Send second welcome message
  uart_SendString(test_msg_1);

  return;
} /* uart_init */
