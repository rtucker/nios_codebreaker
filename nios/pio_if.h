//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  pio_if.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants for pio_if.h
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_PIO_IF__H
#define __LAB_7_PIO_IF__H

#include "nios_std_types.h"   // standard data types

// PIO register offsets (uint32)
#define   PIO_REG_DATA                    0
#define   PIO_REG_DIRECTION               1
#define   PIO_REG_IRQMASK                 2
#define   PIO_REG_EDGECAPTURE             3

// PIO LED masks
#define   PIO_LEDS_OFF_MASK               0x0
#define   PIO_LEDS_RED_MASK               0x1
#define   PIO_LEDS_GREEN_MASK             0x2

// PIO Key Masks
#define   PIO_KEYS_KEY1                   0x1
#define   PIO_KEYS_KEY2                   0x2

// Prototypes
void pio_ssd_update(uint32 value, uint32 enable);
void pio_leds_update(uint32 red, uint32 green);
uint32 pio_key_pressed(uint32 key);
void pio_init();

#endif /* __LAB_7_PIO_IF__H */
