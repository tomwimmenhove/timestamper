--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   20:03:41 03/01/2019
-- Design Name:   
-- Module Name:   /home/tom/grive-Tom.Wimmenhove/Projects/cosmic/hardware/capture_counter/tb_serial_out.vhd
-- Project Name:  capture_counter
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: serial_out
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
 
ENTITY tb_serial_out IS
END tb_serial_out;
 
ARCHITECTURE behavior OF tb_serial_out IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT serial_out
    PORT(
         TXREG : IN  std_logic_vector(7 downto 0);
         SCLK : IN  std_logic;
         CE_N : IN  std_logic;
         SDO : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal TXREG : std_logic_vector(7 downto 0) := (others => '0');
   signal SCLK : std_logic := '0';
   signal CE_N : std_logic := '0';

 	--Outputs
   signal SDO : std_logic;
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: serial_out PORT MAP (
          TXREG => TXREG,
          SCLK => SCLK,
          CE_N => CE_N,
          SDO => SDO
        );

   -- Stimulus process
   stim_proc: process
		variable I: integer;
   begin
		CE_N <= '1';
		wait for 10ns;
		
		TXREG <= "10101010";
		CE_N <= '0';
		for I in 0 to 7 loop
			wait for 10ns;
			SCLK <= '1';
			wait for 10ns;
			SCLK <= '0';
		end loop;
		wait for 10ns;
		CE_N <= '1';
		
		wait for 20ns;

		TXREG <= "01010101";
		CE_N <= '1';
		for I in 0 to 7 loop
			wait for 10ns;
			SCLK <= '1';
			wait for 10ns;
			SCLK <= '0';
		end loop;
		wait for 10ns;

		CE_N <= '0';
		
		wait for 10ns;
		for I in 0 to 7 loop
			wait for 10ns;
			SCLK <= '1';
			wait for 10ns;
			SCLK <= '0';
		end loop;

		CE_N <= '1';
		
		for I in 0 to 7 loop
			wait for 10ns;
			SCLK <= '1';
			wait for 10ns;
			SCLK <= '0';
		end loop;
		wait for 10ns;
		CE_N <= '1';


      wait;
   end process;

END;
