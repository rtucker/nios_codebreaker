//***************************  C Source Code  *****************************
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  lfsr_if.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//    This file implements hardware abstraction functions for accessing
//    the LFSR in the Game System design of lab 7.
//
//*************************************************************************
//*************************************************************************

#include "nios_std_types.h"   // standard data types
#include "system.h"           // BSP-provided definitions
#include <sys/alt_irq.h>      // interrupt-related prototypes

#include "lfsr_if.h"          // defines and constants for hw interfacing
#include "utilities.h"        // useful utilities

// LFSR register pointer
volatile uint16* lfsr = (uint16*)LFSR_16_0_BASE;

//-------------------------------------------------------------------------
// NAME:        lfsr_rand
//
// DESCRIPTION: Retrieves a value from the LFSR PRNG.  This is a 16-bit
//              LFSR, so that's all we'll return.
// ARGUMENTS:   None
// RETURNS:     uint16 random number
//-------------------------------------------------------------------------
uint16 lfsr_rand()
{
  return *(lfsr + LFSR_REG_LFSR);
} /* lfsr_rand */

//-------------------------------------------------------------------------
// NAME:        lfsr_rand_valid
//
// DESCRIPTION: Validity check for LFSR readiness.
// ARGUMENTS:   None
// RETURNS:     uint32.  1 if the hardware LFSR PRNG considers itself
//                       seeded and valid, 0 otherwise.
//-------------------------------------------------------------------------
uint32 lfsr_rand_valid()
{
  uint32 seeded;

  seeded = (uint32)*(lfsr + LFSR_REG_STATUS);
  seeded &= LFSR_REG_STATUS_SEEDED_MASK;
  return seeded;
} /* lfsr_rand_valid */

//-------------------------------------------------------------------------
// NAME:        lfsr_rand_init
//
// DESCRIPTION: Seeds the hardware LFSR PRNG
// ARGUMENTS:   uint16 seed
// RETURNS:     void
//-------------------------------------------------------------------------
void lfsr_rand_init(uint16 seed)
{
  if (seed > 0)   // value must be > 0!!
  {
    *(lfsr + LFSR_REG_SEED)     = seed;
    *(lfsr + LFSR_REG_CONTROL) |= LFSR_REG_CONTROL_RESEED_MASK;
  } /* if */

  return;
} /* lfsr_rand_init */
