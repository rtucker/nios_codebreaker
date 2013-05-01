//***************************  C Source Code  *****************************
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  timer_if.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//    This file implements timer hardware abstraction functions for
//    the Game System design of lab 7.
//
//*************************************************************************
//*************************************************************************

#include "nios_std_types.h"   // standard data types
#include "system.h"           // BSP-provided definitions
#include <sys/alt_irq.h>      // interrupt-related prototypes

#include "timer_if.h"         // defines and constants for hw interfacing
#include "utilities.h"        // useful utilities
#include "pio_if.h"           // PIO interface

volatile  uint16* timer_reg = (uint16*)TIMER_GAME_1SEC_BASE;
uint32    _time_remaining;    // stores our countdown

//-------------------------------------------------------------------------
// NAME:        _timer_isr
//
// DESCRIPTION: Interrupt service routine for one-second tick counter
//-------------------------------------------------------------------------
void _timer_isr(void *context)
{
  if (0 != (*(timer_reg + TIMER32_REG_STATUS) & TIMER32_REG_STATUS_TO_MASK))
  {
    // Decrement the timer, only if it's greater than zero, to avoid
    // wrapping around
    if (0 != _time_remaining)
    {
      _time_remaining--;
    } /* if */
    else
    {
      // Stop the timer because we're at zero.
      timer_countdown_stop();
    } /* else */

    // Send the current count to the SSD
    pio_ssd_update(_time_remaining, TRUE);

    // Clear the TO bit to acknowledge the interrupt
    *(timer_reg + TIMER32_REG_STATUS) = 0;
  } /* if */

  return;
} /* _timer_isr */

//-------------------------------------------------------------------------
// NAME:        timer_countdown_start
//
// DESCRIPTION: Starts counting down at one-second intervals using the
//              hardware timer
// ARGUMENTS:   uint32 start_count, initial value to count down from
// RETURNS:     void
//-------------------------------------------------------------------------
void timer_countdown_start(uint32 start_count)
{
  uint16 reg_val;

  // reset the count
  _time_remaining = start_count;

  // Send it to the SSD
  pio_ssd_update(_time_remaining, TRUE);

  // set up and start the timer: clear TO bit, reset to start value, then
  // enable the interrupt, continuous run, and start the timer
  *(timer_reg + TIMER32_REG_STATUS) = 0;
  *(timer_reg + TIMER32_REG_PERIOD_L) = 1;
  reg_val = *(timer_reg + TIMER32_REG_CONTROL);
  reg_val |= (TIMER32_REG_CONTROL_ITO_MASK | TIMER32_REG_CONTROL_CONT_MASK |
              TIMER32_REG_CONTROL_START_MASK);
  *(timer_reg + TIMER32_REG_CONTROL) = reg_val;
  return;
} /* timer_countdown_start */

//-------------------------------------------------------------------------
// NAME:        timer_countdown_stop
//
// DESCRIPTION: Stops the timer and disables interrupts.
// ARGUMENTS:   None
// RETURNS:     void
//-------------------------------------------------------------------------
void timer_countdown_stop()
{
  uint16 reg_val;

  // Stop the timer, then disable the interrupts
  reg_val = *(timer_reg + TIMER32_REG_CONTROL);
  reg_val |= TIMER32_REG_CONTROL_STOP_MASK;
  reg_val &= ~(TIMER32_REG_CONTROL_ITO_MASK);
  *(timer_reg + TIMER32_REG_CONTROL) = reg_val;

  // Turn off SSDs
  pio_ssd_update(0, FALSE);

  return;
} /* timer_countdown_stop */

//-------------------------------------------------------------------------
// NAME:        timer_remaining
//
// DESCRIPTION: Returns the amount of time left on the countdown clock
// ARGUMENTS:   None
// RETURNS:     uint32, time remaining in seconds
//-------------------------------------------------------------------------
uint32 timer_remaining()
{
  return _time_remaining;
} /* timer_remaining */

//-------------------------------------------------------------------------
// NAME:        timer_expired
//
// DESCRIPTION: Returns TRUE if the timer has exhausted itself, FALSE
//              otherwise.
// ARGUMENTS:   None
// RETURNS:     uint32, FALSE if time remains, TRUE otherwise
//-------------------------------------------------------------------------
uint32 timer_expired()
{
  uint32 result = TRUE;

  if (_time_remaining > 0)
  {
    result = FALSE;
  } /* if */

  return result;
} /* timer_expired */

//-------------------------------------------------------------------------
// NAME:        timer_init
//
// DESCRIPTION: Initializes registers and variables, and registers our
//              interrupt service routine.
// ARGUMENTS:   None
// RETURNS:     void
//-------------------------------------------------------------------------
void timer_init()
{
  _time_remaining = 0;
  *(timer_reg + TIMER32_REG_STATUS) = 0x0;
  *(timer_reg + TIMER32_REG_CONTROL) = TIMER32_REG_CONTROL_STOP_MASK;

  alt_ic_isr_register(TIMER_GAME_1SEC_IRQ_INTERRUPT_CONTROLLER_ID,
                      TIMER_GAME_1SEC_IRQ, _timer_isr, 0, 0);

  return;                      
} /* timer_init */
