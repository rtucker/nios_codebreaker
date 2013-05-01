//***************************  C Source Code  *****************************
//*********  Copyright 2013, Rochester Institute of Technology  ***********
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  codebreaker.c
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//    This is an implementation of the CodeBreaker game.
//
//*************************************************************************
//*************************************************************************

#include <string.h>
#include "nios_std_types.h"         // for standard embedded types
#include "system.h"                 // for Qsys defines

#include "lfsr_if.h"
#include "pio_if.h"
#include "timer_if.h"
#include "uart_if.h"
#include "utilities.h"

#include "codebreaker.h"



//-------------------------------------------------------------------------
// NAME:        check_guess
//
// DESCRIPTION: Checks each character in the guess to determine if it is
//              in the actual secret code, and if so, whether it is in the
//              right position.
// ARGUMENTS:
//    secret  uint8*  pointer to the secret code (null-terminated string)
//    guess   uint8*  pointer to the user's guess (null-terminated string)
//    hint    uint8*  pointer to a place to put the guess (again, string)
// RETURNS:
//    uint32  TRUE or FALSE depending on whether they're a winner
//-------------------------------------------------------------------------
uint32 check_guess(uint8* secret, uint8* guess, uint8* hint)
{
  uint32  result  = FALSE;    /* end result */
  uint32  got_p   = FALSE;
  uint32  got_c   = FALSE;

  // Indices for loops
  uint32  secret_idx;
  uint32  guess_idx;
  uint32  hint_idx = 0;

  for (guess_idx = 0; guess_idx < CB_COLOR_LENGTH; guess_idx++)
  {
    if (NULL == guess[guess_idx])
    {
      // we're out of characters on the input
      result = FALSE;
      hint[hint_idx] = NULL;
      break;
    } /* if null */
    for (secret_idx = 0; secret_idx < CB_COLOR_LENGTH; secret_idx++)
    {
      if (NULL == secret[secret_idx])
      {
        // this is a big problem...
        uart_SendString((uint8*)"secret too short?  aborting check_guess\n");
        return FALSE;
      } /* if on fire */
      if (guess[guess_idx] == secret[secret_idx])
      {
        // we have a match!
        if (guess_idx == secret_idx)
        {
          // in the correct position!
          hint[hint_idx++] = 'P';
          got_p = TRUE;
        } /* if position */
        else
        {
          // not in the correct position!
          hint[hint_idx++] = 'C';
          got_c = TRUE;
        } /* else */
      } /* if match */
    } /* for secret_idx */
  } /* for guess_idx */

  if (got_p && !got_c && (CB_COLOR_LENGTH == hint_idx))
  {
    // We have a winner!
    result = TRUE;
  } /* if winner */

  // null-terminate the hint string
  hint[hint_idx] = NULL;

  return result;
} /* check_guess */

//-------------------------------------------------------------------------
// NAME:        game_loop
//
// DESCRIPTION: Main function of the game itself.  Ensures that the game
//              starts in a fresh state on every round.
// ARGUMENTS:   None
// RETURNS:     void
//-------------------------------------------------------------------------
void game_loop()
{
  uint32  secret_code;
  uint8   secret_code_str[CB_COLOR_LENGTH+1];
  uint8   input_str[UART_RECVBUFFER];
  uint8   hint_str[CB_COLOR_LENGTH+1];

  uint32  loser = FALSE;
  uint32  winner = FALSE;

  // Clear strings
  memset(secret_code_str, 0, sizeof(secret_code_str));
  memset(input_str, 0, sizeof(input_str));
  memset(hint_str, 0, sizeof(hint_str));

  // Set the UART mode to MAIN
  uart_SetMode(UART_MAINMODE);

  // Announce that a new game is starting
  uart_SendString((uint8*)CB_NEWGAME);

  // Wait for key1 press
  pio_key_pressed(1); // clear it
  uart_SendString((uint8*)CB_PRESSKEY1);
  while (!pio_key_pressed(1));

  // Clear LEDs
  pio_leds_update(FALSE, FALSE);

  // Generate secret code
  secret_code = generate_secret_code();
  #ifdef CHEAT_MODE
    // If we're under development, simply output the secret number...
    uart_SendString((uint8*)"Today's secret number is: ");
    to_colorstr(secret_code, (uint8*)secret_code_str);
    uart_SendString((uint8*)secret_code_str);
    uart_SendString((uint8*)"!\n");
  #endif /* CHEAT_MODE */

  // Start game by notifying user, switching to game input mode, and
  // starting the countdown timer.
  uart_SendString((uint8*)CB_GAMESTART);
  uart_SetMode(UART_GAMEMODE);
  timer_countdown_start(CB_COUNTDOWN_TIME);

  // Main gameplay loop
  while (!winner && !loser)
  {
    // Clear the input
    memset(input_str, 0, sizeof(input_str));

    // Display the prompt
    uart_SendString((uint8*)CB_PROMPT);

    // Wait for something to happen...
    while(1)
    {
      // check for incoming stuff from the UART
      uart_RecvString(input_str);

      // strip down to 

      // check for key 2
      if (pio_key_pressed(2))
      {
        uart_SendString((uint8*)CB_YOUGUESSED);
        uart_SendString(input_str);
        uart_SendString((uint8*)"\n");
        break;
      } /* if key 2 pressed */

      // check time remaining
      if (timer_expired())
      {
        // sorry!
        loser = TRUE;
        break;
      } /* if timer_expired */
    } /* while idle */

    if(!loser)
    {
      winner = check_guess((uint8*)secret_code_str, (uint8*)input_str,
                           (uint8*)hint_str);
      if(!winner)
      {
        // Demoralize the opponent
        switch (lfsr_rand() % 4)
        {
          case 0:
            uart_SendString((uint8*)CB_NOTRIGHT1);
            break;
          case 1:
            uart_SendString((uint8*)CB_NOTRIGHT2);
            break;
          case 2:
            uart_SendString((uint8*)CB_NOTRIGHT3);
            break;
          case 3:
            uart_SendString((uint8*)CB_NOTRIGHT4);
            break;
          default:
            // This shouldn't happen with mod 4......
            uart_SendString((uint8*)"Bank error in your favor, collect $200.\n");
            break;
        } /* switch */

        // Display a hint
        uart_SendString((uint8*)CB_YOURHINT);
        uart_SendString((uint8*)hint_str);
        uart_SendString((uint8*)"\n");

      } /* if !winner */
    } /* if !loser */
  } /* while neither winner nor loser */

  // Display either win or lose
  pio_leds_update(loser, winner);
  if(loser)
  {
    timer_countdown_stop();
    uart_SendString((uint8*)CB_TIME_EXPIRED);
  } /* if loser */
  else if (winner)
  {
    timer_countdown_stop();
    uart_SendString((uint8*)CB_WINNER);
  } /* if winner */
  else
  {
    // ??  We shouldn't be here.
    uart_SendString((uint8*)"Error: you have neither won nor lost.");
  } /* else */
} /* game_loop */

//-------------------------------------------------------------------------
// NAME:        main
//
// DESCRIPTION: Main function of codebreaker program.  Handles system setup
//              tasks, briefing of user, and repeated calls to game_loop.
// ARGUMENTS:   None
// RETURNS:     int
//-------------------------------------------------------------------------
int main()
{
  // System initialization tasks
  //
  // Linear Feedback Shift Register PRNG
  // (Seed with the Qsys build timestamp)
  lfsr_rand_init((uint16)SYSID_QSYS_0_TIMESTAMP);
  // Peripheral I/O initialization
  pio_init();
  // Timer initialization
  timer_init();
  // UART initialization
  uart_init();

  // Set up a known initial state
  //
  // Shut off the seven-segment displays and LEDs
  pio_ssd_update(0, FALSE);
  pio_leds_update(FALSE, FALSE);
  // Stop the timer, which shouldn't be running anyway
  timer_countdown_stop();
  // Send the greeting to the user
  uart_SendString((uint8*)CB_WELCOME);
  uart_SendString((uint8*)CB_INSTRUCTIONS);

  // Play the game.  Check for LFSR validity while doing so, to ensure that
  // we have a random initial state...
  while(lfsr_rand_valid())
  {
    game_loop();
  } /* while 1 */

  return 0;
} /* main */
