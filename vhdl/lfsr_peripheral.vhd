--**************************  VHDL Source Code ****************************
--*********  Copyright 2013, Rochester Institute of Technology  ***********
--*************************************************************************
-- vim: set ts=2 sw=2 tw=78 et :
--
--  DESIGNER NAME:  Ryan S. Tucker <rst7983@rit.edu>
--
--       LAB NAME:  Lab 7: Game System
--
--      FILE NAME:  lfsr_peripheral.vhd
--
---------------------------------------------------------------------------
--
--  DESCRIPTION
--
--    This design will implement a 16-bit Galois linear-feedback shift
--    register for pseudorandom number generation.
--
--    About LFSRs:
--      https://en.wikipedia.org/wiki/Linear_feedback_shift_register
--
--    Addresses of this component:
--      0   status    R   bit 0:  1 if LFSR is seeded, 0 if not
--                        bit 1:  1 if seed value is nonzero, 0 if zero
--      1   control    W  bit 0:  1 to reseed LFSR from seed register
--                                (note: status bit 1 MUST be 1 first)
--      2   lfsr      R   current 16-bit LFSR value
--      3   seed      RW  seed value for LFSR
--
---------------------------------------------------------------------------
--
--  REVISION HISTORY
--
--  ______________________________________________________________________
-- |  DATE    | USER | Ver |  Description                                 |
-- |==========+======+=====+==============================================
-- |          |      |     |
-- | 04/21/13 | RST  | 1.0 | Created
-- |          |      |     |
-- |----------|------|-----|----------------------------------------------
--
--*************************************************************************
--*************************************************************************

library IEEE;
use IEEE.std_logic_1164.ALL;
--use IEEE.numeric_std.ALL;
--use IEEE.numeric_std_unsigned.ALL;
use IEEE.std_logic_unsigned.ALL;

entity lfsr_peripheral is
  port (
    -- inputs
    clk     : in  std_logic;
    reset_n : in  std_logic;
    we_n    : in  std_logic;
    be_n    : in  std_logic_vector(1 downto 0);
    a       : in  std_logic_vector(1 downto 0);
    din     : in  std_logic_vector(15 downto 0);
    -- outputs
    dout    : out std_logic_vector(15 downto 0)
  );
end entity lfsr_peripheral;

architecture rtl of lfsr_peripheral is
  -- constants
  constant  STAT_ADDR   : std_logic_vector(1 downto 0) := "00";
  constant  CTRL_ADDR   : std_logic_vector(1 downto 0) := "01";
  constant  LFSR_ADDR   : std_logic_vector(1 downto 0) := "10";
  constant  SEED_ADDR   : std_logic_vector(1 downto 0) := "11";

  constant  READ        : std_logic := '1';
  constant  WRITE       : std_logic := '0';
  constant  RESET       : std_logic := '0';
  constant  BYTE_EN     : std_logic := '0';

  constant  UNSEEDED    : std_logic := '0';
  constant  SEEDED      : std_logic := '1';

  constant  INVALID     : std_logic := '0';
  constant  VALID       : std_logic := '1';

  constant  NOTNOW      : std_logic := '0';
  constant  DOITNOW     : std_logic := '1';

  constant  ZEROS_8     : std_logic_vector := "00000000";

  -- internal signals
  signal    din_h       : std_logic_vector(7 downto 0);
  signal    din_l       : std_logic_vector(7 downto 0);
  signal    be_n_h      : std_logic;
  signal    be_n_l      : std_logic;

  -- registers
  signal    reg_seed_h  : std_logic_vector(7 downto 0);
  signal    reg_seed_l  : std_logic_vector(7 downto 0);

  signal    reg_lfsr    : std_logic_vector(15 downto 0);
  signal    reg_stat    : std_logic_vector(15 downto 0);

  -- control and status flags
  signal    is_seeded   : std_logic := UNSEEDED;
  signal    seed_valid  : std_logic := INVALID;
  signal    ctrl_doseed : std_logic := NOTNOW;

begin
  -- combinational logic: break down long signals into shorter ones, and
  --                      vice versa
  din_h     <= din(15 downto 8);
  din_l     <= din(7 downto 0);
  be_n_h    <= be_n(1);
  be_n_l    <= be_n(0);

  -- test seed validity
  seed_valid  <=  INVALID when (reg_seed_h = ZEROS_8 and reg_seed_l = ZEROS_8)
                          else VALID;

  -- process: seed_register_p
  --  handle writes to the seed register
  --  control inputs: a, we_n, be_n_h
  --  bus output:     din
  --  registers:      reg_seed_h, reg_seed_l
  seed_register_p : process(clk, reset_n) is
  begin
    if (reset_n = RESET) then
      -- Reset seed register to zero
      reg_seed_h    <= (others => '0');
      reg_seed_l    <= (others => '0');
    elsif (rising_edge(clk)) then
      if (a = SEED_ADDR and we_n = WRITE and be_n_h = BYTE_EN) then
        reg_seed_h  <= din_h;
      end if;
      if (a = SEED_ADDR and we_n = WRITE and be_n_l = BYTE_EN) then
        reg_seed_l  <= din_l;
      end if;
    end if;
  end process seed_register_p;

  -- process: lfsr_register_p
  --  implements the linear feedback shift register
  --  control input:  ctrl_doseed
  --  status output:  is_seeded
  --  register:       reg_lfsr
  lfsr_register_p : process(clk, reset_n) is
    variable  carry_bit : std_logic;
    variable  new_lfsr  : std_logic_vector(15 downto 0);
  begin
    if (reset_n = RESET) then
      -- Reset LFSR register to all-ones
      --  (as 0 is an invalid state)
      reg_lfsr      <= (others => '1');
      is_seeded     <= UNSEEDED;
    elsif (rising_edge(clk)) then
      if (ctrl_doseed = DOITNOW) then
        -- replace our LFSR with the seed
        reg_lfsr    <= reg_seed_h & reg_seed_l;
        is_seeded   <=  SEEDED;
      else
        -- recirculating shift right, with carry bit xor'd at
        -- taps: 16, 14, 13, 11
        carry_bit     := reg_lfsr(0);
        new_lfsr(15)  :=                  carry_bit;
        new_lfsr(14)  := reg_lfsr(15)              ;
        new_lfsr(13)  := reg_lfsr(14) xor carry_bit;
        new_lfsr(12)  := reg_lfsr(13) xor carry_bit;
        new_lfsr(11)  := reg_lfsr(12);
        new_lfsr(10)  := reg_lfsr(11) xor carry_bit;
        new_lfsr(9 downto 0) := reg_lfsr(10 downto 1);

        -- update register
        reg_lfsr  <=  new_lfsr;
      end if;
    end if;
  end process lfsr_register_p;

  -- process: ctrl_register_p
  --  implements a write-only control register
  --  control inputs: a, we_n, be_n_l, seed_valid
  --  control output: ctrl_doseed
  ctrl_register_p : process(clk, reset_n) is
  begin
    if (reset_n = RESET) then
      ctrl_doseed <= NOTNOW;
    elsif (rising_edge(clk)) then
      if (a = CTRL_ADDR and we_n = WRITE and be_n_l = BYTE_EN
          and seed_valid = VALID) then
        ctrl_doseed <= DOITNOW;
      else
        ctrl_doseed <= NOTNOW;
      end if;
    end if;
  end process ctrl_register_p;

  -- process: stat_register_p
  --  updates a read-only status register
  --  status inputs:  is_seeded, seed_valid
  --  register:       reg_stat
  stat_register_p : process(clk, reset_n) is
  begin
    if (reset_n = RESET) then
      reg_stat <= (others => '0');
    elsif (rising_edge(clk)) then
      reg_stat <= (others => '0');
      reg_stat(0) <= is_seeded;
      reg_stat(1) <= seed_valid;
    end if;
  end process stat_register_p;

  -- process: register_read_p
  --  selects requested register values to the dout bus
  --  control input:  a
  --  bus output:     dout
  register_read_p : process(clk, reset_n) is
  begin
    if (reset_n = RESET) then
      dout  <= (others => '0');
    elsif (rising_edge(clk)) then
      case a is
        when  STAT_ADDR =>
          dout  <=  reg_stat;
        when  LFSR_ADDR =>
          dout  <=  reg_lfsr;
        when  SEED_ADDR =>
          dout  <=  reg_seed_h & reg_seed_l;
        when  others =>
          dout  <=  (others => '0');
      end case;
    end if;
  end process register_read_p;
end architecture rtl;
