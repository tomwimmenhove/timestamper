--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   19:33:50 03/01/2019
-- Design Name:   
-- Module Name:   /home/tom/grive-Tom.Wimmenhove/Projects/cosmic/hardware/capture_counter/tb_capture_counter.vhd
-- Project Name:  capture_counter
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: capture_counter
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY tb_capture_counter IS
	generic (
		WIDTH: integer := 4
	);
END tb_capture_counter;
 
ARCHITECTURE behavior OF tb_capture_counter IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT capture_counter
	 	generic (
		WIDTH: integer := WIDTH
	);
	 PORT(
         CAPTURE_ENABLE : IN  std_logic;
         CLK : IN  std_logic;
         RST : IN  std_logic;
         CAPT : IN  std_logic;
         RSTCAPT : IN  std_logic;
         LATCH : OUT  std_logic_vector(WIDTH - 1 downto 0);
         INT : OUT  std_logic
        );
    END COMPONENT;

   --Inputs
   signal CAPTURE_ENABLE : std_logic := '0';
   signal CLK : std_logic := '0';
   signal RST : std_logic := '0';
   signal CAPT : std_logic := '0';
   signal RSTCAPT : std_logic := '0';

 	--Outputs
   signal LATCH : std_logic_vector(WIDTH- 1 downto 0);
   signal INT : std_logic;

   -- Clock period definitions
   constant CLK_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: capture_counter PORT MAP (
          CAPTURE_ENABLE => CAPTURE_ENABLE,
          CLK => CLK,
          RST => RST,
          CAPT => CAPT,
          RSTCAPT => RSTCAPT,
          LATCH => LATCH,
          INT => INT
        );

   -- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
		RST <= '1';
		RSTCAPT <= '1';
      wait for CLK_period * 1;
		RST <= '0';
		RSTCAPT <= '0';
		
		CAPTURE_ENABLE <= '1';
		
		wait for CLK_period * 10;

		CAPT <= '1';
		wait for CLK_period * 3;
		CAPT <= '0';
		
		wait for CLK_period;
		
		RSTCAPT <= '1';
		wait for CLK_period * 3;
		RSTCAPT <= '0';

		wait for CLK_period * 14;
		CAPT <= '1';
		wait for CLK_period;
		CAPT <= '0';

		CAPT <= '1';
		wait for CLK_period;
		CAPT <= '0';
		
		RST <= '1';
		wait for CLK_period * 3;
		RST <= '0';
		
		
      wait;
   end process;

END;
