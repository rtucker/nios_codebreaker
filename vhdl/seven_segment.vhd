--**************************************************************************
--************************  VHDL Source Code  ******************************
--**************************************************************************
--
--  DESIGNER NAME:  Ryan Tucker <rst7983@rit.edu>
--
--       LAB NAME:  Library component
--
--      FILE NAME:  seven_segment.vhd
--
----------------------------------------------------------------------------
--
--  DESCRIPTION
--
--  This module converts a four-bit std_logic_vector into a seven-bit
--  active-low vector for a common-anode seven-segment display, as featured
--  on the Altera DE2 board.
--
--  The seven-segment display elements are designated as HEXx_D[0..6]
--  where "x" is a specific digit (numbered 7 through 0).  [0..6] translates
--  to [a..g] in the typical seven-segment display arrangement:
--
--              aaaaa
--             f     b
--             f     b
--              ggggg
--             e     c
--             e     c
--              ddddd
--
--
-- INPUTS:  digit           slv input hex digit (0 through F)
--          enable          sl active-high enable (off = blank)
--          bank            sl bank switch (0 = hex digits, 1 = special)
-- OUTPUTS: sevenseg        slv seven-segment display map
--
-- Also requires constants SS_ZERO, SS_ONE, ... SS_NINE, as well
-- as SS_A, SS_B, .. SS_F for hex conditions and SS_SMALL_R for errors.
--
-- Outputs for a given input hex value and bank toggle:
--  (blank = same for both banks)
--
--                              Input Hex Code
--  Bank    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
-------------------------------------------------------------------------
--  0       0   1   2   3   4   5   6   7   8   9   A   B   C   d   E   F
--  1       o                           L                   c
--
-------------------------------------------------------------------------------
--
--  REVISION HISTORY
--
--  _______________________________________________________________________
-- |  DATE    | USER | Ver |  Description                                  |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 01/10/11 | RST  | 1.0 | Created as part of Lab 5 code.
-- |          |      |     |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 01/16/11 | RST  | 1.1 | Generalized for use in Lab 6.
-- |          |      |     |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 01/17/11 | RST  | 1.2 | Add an enable input to blank the display.
-- |          |      |     |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 01/24/11 | RST  | 1.3 | Add enable input to process sensitivity list
-- |          |      |     |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 12/08/12 | RST  | 1.4 | Add 'bank' input to allow additional characters
-- |          |      |     |
-- |==========+======+=====+================================================
-- |          |      |     |
-- | 12/11/12 | RST  | 1.5 | Correct SS_d pattern
-- |          |      |     |

--
--**************************************************************************
--**************************************************************************

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY seven_segment IS
    PORT (
        digit       : IN  std_logic_vector(3 DOWNTO 0);
        enable      : IN  std_logic;
        bank        : IN  std_logic;
        --
        sevenseg    : OUT std_logic_vector(6 DOWNTO 0)
        );
END ENTITY seven_segment;

ARCHITECTURE behave OF seven_segment IS
    -- CONSTANTS for (active-LOW) seven-segment display outputs
    -- Uses "typical" (clockwise from top) a->g lettering:
    --                                                         gfedcba
    CONSTANT SS_OFF         : std_logic_vector(6 DOWNTO 0) := "1111111";
    CONSTANT SS_ZERO        : std_logic_vector(6 DOWNTO 0) := "1000000";
    CONSTANT SS_SMALL_o     : std_logic_vector(6 DOWNTO 0) := "0100011";
    CONSTANT SS_ONE         : std_logic_vector(6 DOWNTO 0) := "1111001";
    CONSTANT SS_TWO         : std_logic_vector(6 DOWNTO 0) := "0100100";
    CONSTANT SS_THREE       : std_logic_vector(6 DOWNTO 0) := "0110000";
    CONSTANT SS_FOUR        : std_logic_vector(6 DOWNTO 0) := "0011001";
    CONSTANT SS_FIVE        : std_logic_vector(6 DOWNTO 0) := "0010010";
    CONSTANT SS_SIX         : std_logic_vector(6 DOWNTO 0) := "0000010";
    CONSTANT SS_SEVEN       : std_logic_vector(6 DOWNTO 0) := "1111000";
    CONSTANT SS_EIGHT       : std_logic_vector(6 DOWNTO 0) := "0000000";
    CONSTANT SS_NINE        : std_logic_vector(6 DOWNTO 0) := "0011000";
    CONSTANT SS_A           : std_logic_vector(6 DOWNTO 0) := "0001000";
    CONSTANT SS_b           : std_logic_vector(6 DOWNTO 0) := "0000011";
    CONSTANT SS_C           : std_logic_vector(6 DOWNTO 0) := "1000110";
    CONSTANT SS_SMALL_c     : std_logic_vector(6 DOWNTO 0) := "0100111";
    CONSTANT SS_d           : std_logic_vector(6 DOWNTO 0) := "0100001";
    CONSTANT SS_E           : std_logic_vector(6 DOWNTO 0) := "0000110";
    CONSTANT SS_F           : std_logic_vector(6 DOWNTO 0) := "0001110";
    CONSTANT SS_L           : std_logic_Vector(6 DOWNTO 0) := "1000111";
    CONSTANT SS_SMALL_R     : std_logic_vector(6 DOWNTO 0) := "0101111";
BEGIN
    display: PROCESS (bank, digit, enable) IS
    BEGIN
        IF (enable = '1') THEN
            CASE digit IS
                WHEN "0000" =>
                    CASE bank IS
                        WHEN '1' =>
                            sevenseg <= SS_SMALL_o;
                        WHEN OTHERS =>
                            sevenseg <= SS_ZERO;
                    END CASE;
                WHEN "0001" =>
                    sevenseg <= SS_ONE;
                WHEN "0010" =>
                    sevenseg <= SS_TWO;
                WHEN "0011" =>
                    sevenseg <= SS_THREE;
                WHEN "0100" =>
                    sevenseg <= SS_FOUR;
                WHEN "0101" =>
                    sevenseg <= SS_FIVE;
                WHEN "0110" =>
                    sevenseg <= SS_SIX;
                WHEN "0111" =>
                    CASE bank IS
                        WHEN '1' =>
                            sevenseg <= SS_L;
                        WHEN OTHERS =>
                            sevenseg <= SS_SEVEN;
                    END CASE;
                WHEN "1000" =>
                    sevenseg <= SS_EIGHT;
                WHEN "1001" =>
                    sevenseg <= SS_NINE;
                WHEN "1010" =>
                    sevenseg <= SS_A;
                WHEN "1011" =>
                    sevenseg <= SS_B;
                WHEN "1100" =>
                    CASE bank IS
                        WHEN '1' =>
                            sevenseg <= SS_SMALL_c;
                        WHEN OTHERS =>
                            sevenseg <= SS_C;
                    END CASE;
                WHEN "1101" =>
                    sevenseg <= SS_D;
                WHEN "1110" =>
                    sevenseg <= SS_E;
                WHEN "1111" =>
                    sevenseg <= SS_F;
                WHEN OTHERS =>
                    sevenseg <= SS_SMALL_R;
            END CASE;
        ELSE
            sevenseg <= SS_OFF;
        END IF;
    END PROCESS display;
END ARCHITECTURE behave;
