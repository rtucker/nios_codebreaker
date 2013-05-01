//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  lfsr_if.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants for lfsr_if.c.
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_LFSR_IF__H
#define __LAB_7_LFSR_IF__H

#include "nios_std_types.h"   // standard data types

// LFSR register offsets (uint16) and masks
#define   LFSR_REG_STATUS                 0
#define   LFSR_REG_CONTROL                1
#define   LFSR_REG_LFSR                   2
#define   LFSR_REG_SEED                   3
#define   LFSR_REG_STATUS_SEEDED_MASK     0x1
#define   LFSR_REG_STATUS_SEEDVALID_MASK  0x2
#define   LFSR_REG_CONTROL_RESEED_MASK    0x1

// Prototypes for public functions
uint16 lfsr_rand();
uint32 lfsr_rand_valid();
void lfsr_rand_init(uint16 seed);

#endif /* __LAB_7_LFSR_IF__H */
