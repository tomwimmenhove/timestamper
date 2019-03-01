----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    18:29:33 02/26/2019 
-- Design Name: 
-- Module Name:    serial_out - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;

entity serial_out is
	generic (
		TXWIDTH: integer := 8;
		BMUXWIDTH: integer := 5
	);
	
	port (
		TXREG: in std_logic_vector(TXWIDTH - 1 downto 0);
		SCLK: in std_logic;
		CE_N: in std_logic;
		SDO: out std_logic
	);
end serial_out;

architecture Behavioral of serial_out is
	signal BMUX: std_logic_vector(BMUXWIDTH - 1 downto 0);
begin
	--SDO <= TXREG(TXWIDTH - to_integer(unsigned(BMUX)) - 1);

	SpiOutProcess: process(SCLK, CE_N, BMUX)
	begin
		if CE_N = '1' then
			BMUX <= (others => '0');
		else
			if falling_edge(SCLK) then -- CPOL == CPHA == 0
				BMUX <= BMUX + 1;
			end if;
		end if;
		
		if to_integer(unsigned(BMUX)) < TXWIDTH then
			SDO <= TXREG(TXWIDTH - to_integer(unsigned(BMUX)) - 1);
		end if;
	end process;
end Behavioral;
