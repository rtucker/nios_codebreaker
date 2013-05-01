//***************************  C Source Code  *****************************
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  utilities.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants/prototypes for utilities.c
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_UTILITIES__H
#define __LAB_7_UTILITIES__H

#include "nios_std_types.h"   // standard data types

// prototypes for public functions
uint32 convert_to_bcd(uint16 number);
uint8 to_color(uint8 number);
void to_colorstr(uint32 number, uint8* color_string);
uint32 generate_secret_code();

#endif /* __LAB_7_UTILITIES__H */
