//***************************  C Source Code  *****************************
//*************************************************************************
// vim: set ts=2 sw=2 tw=78 et :
//
//  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
//
//       LAB NAME:  Lab 7: Game System
//
//      FILE NAME:  codebreaker.h
//
//-------------------------------------------------------------------------
//
//  DESCRIPTION
//
//      This file defines various constants for codebreaker.c.
//
//*************************************************************************
//*************************************************************************

#ifndef __LAB_7_CODEBREAKER__H
#define __LAB_7_CODEBREAKER__H

#include "nios_std_types.h"   // standard data types

// Cheat mode: if defined, the secret code will be disclosed to the user at
//             the start of the game.  (Pretty much the ultimate cheat...)
#define CHEAT_MODE

// Magic numbers
#define CB_COLOR_LENGTH 4       // Number of colors in the code
#define CB_POSSIBLE_COLORS 6    // Number of colors available
#define CB_COUNTDOWN_TIME 60    // How long the user gets to play

// Messages
#define CB_WELCOME "\n" \
        "--------------------------------------------------------------\n" \
        "Welcome to the CodeBreaker Game!\n" \
        "--------------------------------------------------------------\n"

#define CB_INSTRUCTIONS "\n" \
  "It's final exam week and you have to get into the lab.  It's exactly\n" \
  "one minute before the lab closes for the night, but if you get in there\n" \
  "before your keycard stops working, you're golden.\n" \
  "\n" \
  "You try to swipe your keycard, but you notice the lock has been changed!\n" \
  "You read the note:\n" \
  "\n" \
  "   THE KEYCARD READER IS BROKEN, SO HERE'S HOW TO GET INTO THE LAB:\n" \
  "     1. There are six buttons: Green, Blue, Red, Orange, Yellow, White\n" \
  "     2. Enter the correct four-color sequence, which is available from\n" \
  "        the department office between 9am and 4pm.\n" \
  "     3. If you get the code wrong, the lock will give you a hint for each\n" \
  "        color you entered: C if it was the correct color in the wrong\n" \
  "        position, or P if it is the correct color in the correct position.\n" \
  "   WE APOLOGIZE FOR THE INCONVENIENCE.  WITH LOVE, RIT FACILITIES.\n" \
  "\n" \
  "You begin to panic, but you glance at the clock and you realize you don't\n" \
  "have any time to panic.  So, you start trying to break the code...\n" \
  "\n" \
  "   HOW TO DO IT:\n" \
  "     1. Enter a four-letter code at the GUESS> prompt, and hit Enter.\n" \
  "         Valid letters are: G, B, R, O, Y, W\n" \
  "     2. Press KEY2 to try to open the door.\n" \
  "     3. If the door doesn't open, you'll get a hint:\n" \
  "         GUESS> ROYG\n" \
  "          Hint: CCCP\n" \
  "        This means that the G is in the right place, and R, O, and Y are\n" \
  "        all part of the code but are in the wrong position.  So, your next\n" \
  "        guess should have R, O, and Y in it (in a different order!) with\n" \
  "        G as the last letter.\n" \
  "     4. If the door does open, you pass the class.\n" \
  "     5. If 60 seconds expires, the lab is closed and you fail the class.\n" \
  "   GOOD LUCK!\n" \
  "\n"

#define CB_NEWGAME   "I'm ready to break the code!  Are you?\n"
#define CB_PRESSKEY1 "--> Press KEY1 to continue...! <--\n"
#define CB_GAMESTART "Let's go!\n" \
                     "You have 60 SECONDS to guess the color pattern before the lab closes.\n"
#define CB_PROMPT    "GUESS> "
#define CB_YOUGUESSED "\n--> You guessed: "
#define CB_NOTRIGHT1 "That wasn't much of a guess.  Give it another shot.\n"
#define CB_NOTRIGHT2 "It's a good thing they're offering this class next year.  Try again.\n"
#define CB_NOTRIGHT3 "Close only counts in horseshoes and hand grenades, but not here.  Guess again.\n"
#define CB_NOTRIGHT4 "One thing is for sure: You did not guess the code.\n"
#define CB_YOURHINT  "Your hint is: "
#define CB_TIME_EXPIRED "\n" \
        "--------------------------------------------------------------\n" \
        "                      YOU WERE TOO SLOW!\n" \
        "    The timer expired and the lab is closed. You tried your\n" \
        "              best, but better luck next semester!\n" \
        "--------------------------------------------------------------\n"
#define CB_WINNER "\n" \
        "--------------------------------------------------------------\n" \
        "                      THE DOOR UNLOCKS!\n" \
        " Thanks to your wicked color-guessing skills, the lab door has\n" \
        "      opened, and your grade has been saved. Good work!\n" \
        "--------------------------------------------------------------\n"

#endif /* __LAB_7_CODEBREAKER__H */
