--*****************************************************************************
--***************************  VHDL Source Code  ******************************
--*********  Copyright 1943, Rochester Institute of Technology  ***************
--*****************************************************************************
-- vim: set ts=2 sw=2 tw=78 et :
--
--  DESIGNER NAME:  Ryan Tucker <rst7983@rit.edu>
--
--       LAB NAME:  Lab 7: Game System
--
--      FILE NAME:  codebreaker_top.vhd
--
-------------------------------------------------------------------------------
-- Description:
--      This module is the top level module for the CodeBreaker game system
--
--*****************************************************************************
--*****************************************************************************

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

library work;

-------------------------------------------------------------------------------
--                             Port Declarations                             --
-------------------------------------------------------------------------------
entity codebreaker_top is
  port (
    -- Inputs
    CLOCK_50              : in std_logic;
    KEY                   : in std_logic_vector (3 downto 0);

    -- Outputs
    LEDG                  : out std_logic_vector(7 downto 0);
    LEDR                  : out std_logic_vector(17 downto 0);
    HEX7                  : out std_logic_vector(6 downto 0);
    HEX6                  : out std_logic_vector(6 downto 0);
    HEX5                  : out std_logic_vector(6 downto 0);
    HEX4                  : out std_logic_vector(6 downto 0);
    HEX3                  : out std_logic_vector(6 downto 0);
    HEX2                  : out std_logic_vector(6 downto 0);
    HEX1                  : out std_logic_vector(6 downto 0);
    HEX0                  : out std_logic_vector(6 downto 0)
    );
end codebreaker_top;

architecture structure of codebreaker_top is
  component nios_system
    port (
      clk_clk                 : in  std_logic;
      reset_reset_n           : in  std_logic;
      pio_keys_export         : in  std_logic_vector(1 downto 0);
      --
      led_toggle_pulse_export : out std_logic;
      pio_countdown_export    : out std_logic_vector(7 downto 0);
      pio_leds_export         : out std_logic_vector(1 downto 0)
    );
  end component nios_system;

  signal  led_toggle_pulse    : std_logic;
  signal  led_gate            : std_logic := '0';
  signal  leds                : std_logic_vector(1 downto 0);
  signal  countdown           : std_logic_vector(7 downto 0);
begin

  NiosII : nios_system
    port map(
      clk_clk                     => clock_50,
      reset_reset_n               => key(0),
      pio_keys_export             => key(2 downto 1),
      --
      led_toggle_pulse_export     => led_toggle_pulse,
      pio_countdown_export        => countdown,
      pio_leds_export             => leds
    );

  -- process: led_gate_p
  --  updates the LED toggle gate signal on led toggle timers
  led_gate_p  : process (clock_50, key(0)) is
  begin 
    if (key(0) = '0') then
      led_gate <= '0';
    elsif (rising_edge(clock_50)) then
      if (led_toggle_pulse = '1') then
        led_gate <= not led_gate;
      end if;
    end if;
  end process led_gate_p;

  -- combinational logic for controlling the LEDs
  ledr <= (others => '1') when (leds(0) = '1' and led_gate = '1') -- LOSER
                          else (others => '0');

  ledg <= (others => '1') when (leds(1) = '1' and led_gate = '1') -- WINNER
                          else (others => '0');

  -- force unused 7-seg displays off
  hex7 <= (others => '1');
  hex6 <= (others => '1');
  hex5 <= (others => '1');
  hex4 <= (others => '1');
  hex3 <= (others => '1');
  hex2 <= (others => '1');

  ssd_tens: entity work.seven_segment port map (
    digit => "0" & countdown(6 downto 4),
    enable => countdown(7),
    bank => '0',
    sevenseg => hex1
  );

  ssd_ones: entity work.seven_segment port map (
    digit => countdown(3 downto 0),
    enable => countdown(7),
    bank => '0',
    sevenseg => hex0
  );


end structure;
