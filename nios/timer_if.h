//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  hardware_if.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants for timer_if.h
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_TIMER_IF__H
#define __LAB_7_TIMER_IF__H

#include "nios_std_types.h"   // standard data types

// 32-bit timer register offsets (uint16)
#define   TIMER32_REG_STATUS              0
#define   TIMER32_REG_CONTROL             2
#define   TIMER32_REG_PERIOD_L            4
#define   TIMER32_REG_PERIOD_H            6
#define   TIMER32_REG_SNAP_L              8
#define   TIMER32_REG_SNAP_H              10
#define   TIMER32_REG_STATUS_TO_MASK      0x1
#define   TIMER32_REG_STATUS_RUN_MASK     0x2
#define   TIMER32_REG_CONTROL_ITO_MASK    0x1
#define   TIMER32_REG_CONTROL_CONT_MASK   0x2
#define   TIMER32_REG_CONTROL_START_MASK  0x4
#define   TIMER32_REG_CONTROL_STOP_MASK   0x8

// Prototypes
void timer_countdown_start(uint32 start_count);
void timer_countdown_stop();
uint32 timer_remaining();
uint32 timer_expired();
void timer_init();

#endif /* __LAB_7_TIMER_IF__H */
