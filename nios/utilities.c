//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  utilities.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file contains various things which didn't fit into other
//      files.
//
//*************************************************************************
//*************************************************************************

#include "nios_std_types.h"   // standard data types
#include "codebreaker.h"      // for CB_* defines
#include "lfsr_if.h"          // for random number generation
#include "utilities.h"

//-------------------------------------------------------------------------
// NAME:        _is_in
//
// DESCRIPTION: Used by secret code generation to detect if a given
//              4-bit segment is already present in the code.
// ARGUMENTS:   uint32 guess        (truncated to lower 4 bits)
//              uint32* secret_code (scanned in 4-bit chunks)
// RETURNS:     0 if guess is not found in secret_code,
//              > 0 if it is (with 4-bit-aligned index of match)
//-------------------------------------------------------------------------
uint32 _is_in(uint32 guess, uint32* secret_code)
{
  uint32 result = 0;    /* end result */

  uint32 i;
  uint32 code_part;

  // Remove extraneous bits from the guess
  guess &= 0xF;

  for (i = 0; i < CB_COLOR_LENGTH; i++)
  {
    // Isolate a four-bit slot of the secret code.
    code_part = (*secret_code >> i*4);
    code_part &= 0xF;
    // Test for a match.
    if (guess == code_part)
    {
      result = i+1;
      break;
    } /* if */
  } /* for i */

  return result;
} /* _is_in */

//*************************************************************************
// convert_to_bcd
//*************************************************************************
// Converts a value to a binary-coded decimal bitstring, with each four
// bits corresponding to an integer value between 0 and 9.
//
// Loops over the input, extracting the least-significant digit and
// converting it to binary one place at a time.
//
// INPUTS:
//  number      uint16 binary value
// RETURNS: uint32 with bits 31..20 hardcoded zero
uint32 convert_to_bcd(uint16 number)
{
  uint32 bcd_num     = 0;
  uint16 shifted_num = 0;
  uint32 digit       = 0;
  uint32 loop_count  = 0;

  while (0 != number)   // Loop until there is nothing left to convert
  {
    shifted_num = number / 10;                  // Drop LSD
    digit       = number - (shifted_num * 10);  // Isolate LSD
    bcd_num    |= (digit << (loop_count*4));    // Shift BCD into place
    loop_count += 1;
    number      = shifted_num;
  } /* while */

  return bcd_num;
} /* convert_to_bcd */

//-------------------------------------------------------------------------
// NAME:        to_color
//
// DESCRIPTION: Given a uint8 number between 0 and CB_POSSIBLE_COLORS-1,
//              returns a uint8 character representation of that
//              particular color.  Returns X on invalid input.
// ARGUMENTS:   uint8 number, the number to convert.
// RETURNS:     uint8, color character
//-------------------------------------------------------------------------
uint8 to_color(uint8 number)
{
  uint8 color;

  switch (number)
  {
    case 0:
      color = 'G';  // Green
      break;
    case 1:
      color = 'B';  // Blue
      break;
    case 2:
      color = 'R';  // Red
      break;
    case 3:
      color = 'O';  // Octarine
      break;
    case 4:
      color = 'Y';  // Yellow
      break;
    case 5:
      color = 'W';  // White
      break;
    default:
      color = 'X';  // invalid
      break;
  } /* switch */

  return color;
} /* to_color */

//-------------------------------------------------------------------------
// NAME:        to_colorstr
//
// DESCRIPTION: Converts a base-six secret code to a string of letters,
//              used during secret code computation.
// ARGUMENTS:   uint32 number, the number to convert (with digits on 4-bit
//                             alignments
//              uint8* color_string, a pointer to a string that will receive
//                                   the converted value
// RETURNS:     void                                  
//-------------------------------------------------------------------------
void to_colorstr(uint32 number, uint8* color_string)
{
  uint8 color_digit;
  uint8 i;

  for (i = 0; i < CB_COLOR_LENGTH; i++)
  {
    // isolate the ith digit
    color_digit = (number >> i*4) & 0xF;
    color_string[i] = to_color(color_digit);
  } /* for */

  // null-terminate the string to avoid catastrophe
  color_string[CB_COLOR_LENGTH] = 0;

  return;
} /* to_colorstr */

//-------------------------------------------------------------------------
// NAME:        generate_secret_code
//
// DESCRIPTION: Generates a secret code, with each four-bit chunk being
//              unique within the code.  Limits the possible four-bit chunk
//              values to CB_POSSIBLE_COLORS (default 6) and generates
//              CB_COLOR_LENGTH chunks (default 4).
// ARGUMENTS:   None
// RETURNS:     uint32 secret code, as described above
//-------------------------------------------------------------------------
uint32 generate_secret_code()
{
  uint32 secret_code    = 0;      /* end result */

  uint32 random_number;
  uint32 shifted_num    = 0;
  uint32 digit          = 0;
  uint32 loop_count     = 0;
  uint32 got_it         = FALSE;

  // Pick a random number, and convert it into base 6.  Keep doing this until
  // we find a number we haven't already picked.
  for (loop_count = 0; loop_count < CB_COLOR_LENGTH; loop_count++)
  {
    got_it = FALSE;
    while (!got_it)
    {
      random_number = (uint32)lfsr_rand();
      shifted_num   = random_number / CB_POSSIBLE_COLORS;
      digit         = random_number - (shifted_num * CB_POSSIBLE_COLORS);

      if (0 == _is_in(digit, &secret_code))
      {
        // we haven't used this digit before
        got_it = TRUE;
      } /* if */
    } /* while !got_it */
    secret_code  |= (digit << (loop_count*4));
  } /* for loop_count */

  return secret_code;
} /* generate_secret_code */
