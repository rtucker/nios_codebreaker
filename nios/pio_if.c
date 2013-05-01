//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  pio_if.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//    This file implements PIO hardware abstraction functions for
//    the Game System design of lab 7.
//
//*************************************************************************
//*************************************************************************

#include "nios_std_types.h"   // standard data types
#include "system.h"           // BSP-provided definitions
#include <sys/alt_irq.h>      // interrupt-related prototypes

#include "pio_if.h"           // defines and constants for hw interfacing
#include "utilities.h"        // useful utilities

// variables for latching button state
uint32  key1_pressed;
uint32  key2_pressed;

// pointers to our peripherals
volatile uint32* pio_keys = (uint32*)PIO_KEYS_BASE;
volatile uint32* pio_bcd  = (uint32*)PIO_COUNTDOWN_BASE;
volatile uint32* pio_led  = (uint32*)PIO_LEDS_BASE;

//-------------------------------------------------------------------------
// NAME:        _pio_keys_isr
//
// DESCRIPTION: Interrupt service routine for our buttons
//-------------------------------------------------------------------------
void _pio_keys_isr(void *context)
{
  // Test for button presses
  if (0 != (*(pio_keys + PIO_REG_EDGECAPTURE) & PIO_KEYS_KEY1))
  {
    key1_pressed = TRUE;
  } /* if key1 */
  if (0 != (*(pio_keys + PIO_REG_EDGECAPTURE) & PIO_KEYS_KEY2))
  {
    key2_pressed = TRUE;
  } /* if key2 */

  // Reset the register
  *(pio_keys + PIO_REG_EDGECAPTURE) = (PIO_KEYS_KEY1 | PIO_KEYS_KEY2);

  return;
} /* _pio_keys_isr */

//-------------------------------------------------------------------------
// NAME:        pio_ssd_update
//
// DESCRIPTION: Pushes new digits to the seven-segment display.
//              Accepts "normal" input, does NOT need to be BCD
//              already.
// ARGUMENTS:   uint32 value, a number to push to the displays.
//              uint32 enable, > 0 to drive the displays
// RETURNS:     void
//-------------------------------------------------------------------------
void pio_ssd_update(uint32 value, uint32 enable)
{
  uint8 bcd_val = 0;

  if (0 != enable)
  {
    // convert value to BCD, then ensure bit 7 is high
    bcd_val  = (uint8)convert_to_bcd((uint16)value);
    bcd_val |= (1 << 7);
  } /* if */
  else
  {
    // Ensure bit 7 is low
    bcd_val  = 0x0;
  } /* else */

  // Send the BCD value to the hex displays
  *(pio_bcd + PIO_REG_DATA) = bcd_val;

  return;
} /* pio_ssd_update */

//-------------------------------------------------------------------------
// NAME:        pio_leds_update
//
// DESCRIPTION: Pushes new state to the LEDs depending on color values
// ARGUMENTS:   uint32 red, > 0 to blink the red LEDs
//              uint32 green, > 0 to blink the green LEDs
// RETURNS:     void
//-------------------------------------------------------------------------
void pio_leds_update(uint32 red, uint32 green)
{
  uint32  tmpmask = PIO_LEDS_OFF_MASK;

  if (red)    tmpmask |= PIO_LEDS_RED_MASK;
  if (green)  tmpmask |= PIO_LEDS_GREEN_MASK;

  // Push new value to the LEDs
  *(pio_led + PIO_REG_DATA) = tmpmask;

  return;
} /* pio_leds_update */

//-------------------------------------------------------------------------
// NAME:        pio_key_pressed
// DESCRIPTION: Checks to see if a key has been pressed.  Resets on each
//              read.
// ARGUMENTS:   Key number to check (1 or 2)
// RETURNS:     uint32, TRUE if the key has been pressed since the last
//                      read; FALSE otherwise.  FALSE on invalid key.
//-------------------------------------------------------------------------
uint32 pio_key_pressed(uint32 key)
{
  uint32 result = FALSE;

  switch(key)
  {
    case 1:
      result = key1_pressed;
      key1_pressed = FALSE;
      break;
    case 2:
      result = key2_pressed;
      key2_pressed = FALSE;
      break;
  } /* switch */

  return result;
} /* pio_key_pressed */

//-------------------------------------------------------------------------
// NAME:        pio_init
//
// DESCRIPTION: Initialize status variables and set up the PIO ISR
// ARGUMENTS:   None
// RETURNS:     void
//-------------------------------------------------------------------------
void pio_init()
{
  // Reset our internal flags
  key1_pressed = FALSE;
  key2_pressed = FALSE;

  /* Clear the edge capture register with all-ones */
  *(pio_keys + PIO_REG_EDGECAPTURE) = -1;

  // Register and enable interrupts
  alt_ic_isr_register(PIO_KEYS_IRQ_INTERRUPT_CONTROLLER_ID,
                      PIO_KEYS_IRQ,
                      _pio_keys_isr, 0, 0);
  *(pio_keys + PIO_REG_IRQMASK) = (PIO_KEYS_KEY1 | PIO_KEYS_KEY2);

  return;
} /* pio_init */
